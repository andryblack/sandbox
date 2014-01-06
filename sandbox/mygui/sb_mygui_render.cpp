//
//  sb_mygui_render.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 25/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_mygui_render.h"
#include <ghl_render.h>
#include "../sb_data.h"
#include "../sb_texture.h"
#include <sbstd/sb_algorithm.h>
#include "../sb_resources.h"
#include "../sb_matrix4.h"
#include <ghl_image.h>
#include <ghl_texture.h>


namespace Sandbox {
    
    
    namespace mygui {
        
        
        class VertexBuffer : public MyGUI::IVertexBuffer {
        public:
            VertexBuffer() {
                m_data = new VectorData<MyGUI::Vertex>();
            }
            ~VertexBuffer() {
                m_data->Release();
            }
            virtual void setVertexCount(size_t _value) {
                m_data->vector().resize(_value);
            }
            virtual size_t getVertexCount() {
                return m_data->vector().size();
            }
            
            virtual MyGUI::Vertex* lock() {
                return m_data->vector().data();
            }
            virtual void unlock() {
                
            }
            const void* GetData() {
                return m_data->GetData();
            }
        private:
            VectorData<MyGUI::Vertex>*  m_data;
        };
        
        class RenderManager::Texture : public MyGUI::ITexture {
        public:
            Texture( const sb::string& name, Resources* res) : m_name(name),m_resources(res),m_image(0) {
                
            }
            virtual const std::string& getName() const {
                return m_name;
            }
            
            virtual void createManual(int _width, int _height, MyGUI::TextureUsage _usage, MyGUI::PixelFormat _format) {
                if (_format == MyGUI::PixelFormat::R8G8B8A8) {
                    m_texture = m_resources->CreateTexture(_width, _height, true, 0);
                }
            }
            
            virtual void loadFromFile(const std::string& _filename) {
                m_texture = m_resources->GetTexture(_filename.c_str(), false);
            }
            
            virtual void saveToFile(const std::string& _filename) {
                
            }
          
            virtual void destroy() {
                m_texture.reset();
            }
            
            virtual void* lock(MyGUI::TextureUsage _access) {
                if (!m_texture) return 0;
                m_image = GHL_CreateImage(getWidth(), getHeight(), GHL::IMAGE_FORMAT_RGBA);
                return m_image->GetDataPtr();
            }
            
            virtual void unlock() {
                if (m_image && m_texture) {
                    GHL::Texture* tex = m_texture->Present(m_resources);
                    tex->SetData(0, 0, m_image,0);
                    m_image->Release();
                    m_image = 0;
                    tex->DiscardInternal();
                }
            }
            
            virtual bool isLocked() {
                return m_image!=0;
            }
            
            virtual int getWidth() {
                return m_texture ? m_texture->GetWidth() : 0;
            }
            virtual int getHeight() {
                return m_texture ? m_texture->GetHeight() : 0;
            }
            
            virtual MyGUI::PixelFormat getFormat() {
                return MyGUI::PixelFormat::R8G8B8A8;
            }
            virtual MyGUI::TextureUsage getUsage() {
                return MyGUI::TextureUsage::Default;
            }
            virtual size_t getNumElemBytes() {
                return 0;
            }
            const GHL::Texture* Present() {
                return m_texture ? m_texture->Present(m_resources) : reinterpret_cast<GHL::Texture*>((void*)(0));
            }
        private:
            sb::string  m_name;
            int m_width;
            int m_height;
            TexturePtr  m_texture;
            Resources*  m_resources;
            GHL::Image* m_image;
        };
        
        
        RenderManager::RenderManager( Resources* resources ) : m_resources(resources) {
            m_view_size.set(m_resources->GetRender()->GetWidth(),
                            m_resources->GetRender()->GetHeight());
            m_rt_info.maximumDepth = 1;
            m_rt_info.hOffset = 0;//-m_view_size.width/2;
            m_rt_info.vOffset = 0;//-m_view_size.height/2;
            m_rt_info.aspectCoef = float(m_view_size.height) / float(m_view_size.width);
            m_rt_info.pixScaleX = 0.5f;//0.5f;
            m_rt_info.pixScaleY = -0.5f;//-0.5f;
            
            onResizeView(m_view_size);
        }
        
        MyGUI::IVertexBuffer* RenderManager::createVertexBuffer() {
            return new VertexBuffer();
        }
        
        void RenderManager::destroyVertexBuffer(MyGUI::IVertexBuffer* _buffer) {
            delete _buffer;
        }
        
        MyGUI::ITexture* RenderManager::createTexture(const std::string& _name) {
            sb_assert(m_textures.find(_name)==m_textures.end());
            Texture* tex = new Texture(_name,m_resources);
            m_textures[_name] = tex;
            return tex;
        }
        
        void RenderManager::destroyTexture(MyGUI::ITexture* _texture) {
            if (!_texture) return;
            Texture* tex = reinterpret_cast<Texture*>(_texture);
            sb::map<sb::string, Texture*>::iterator it = m_textures.find(tex->getName());
            sb_assert(it!=m_textures.end());
            m_textures.erase(it);
            delete tex;
        }
        
        MyGUI::ITexture* RenderManager::getTexture(const std::string& _name) {
            sb::map<sb::string, Texture*>::iterator it = m_textures.find(_name);
            if (it!=m_textures.end()) {
                return it->second;
            }
            return 0;
        }
        
        const MyGUI::IntSize& RenderManager::getViewSize() const {
            return m_view_size;
        }
        
        MyGUI::VertexColourType RenderManager::getVertexFormat() {
            return MyGUI::VertexColourType::ColourABGR;
        }
        
        bool RenderManager::isFormatSupported(MyGUI::PixelFormat _format, MyGUI::TextureUsage _usage) {
            return !_usage.isValue(MyGUI::TextureUsage::Write);
        }
        
#if MYGUI_DEBUG_MODE == 1
        bool RenderManager::checkTexture(MyGUI::ITexture* _texture) {
            return true;
        }
#endif
        
        
        void RenderManager::begin() {
            GHL::Render* r = m_resources->GetRender();
            if (r) {
                Matrix4f m = Matrix4f::ortho(0, m_view_size.width, m_view_size.height, 0,-1, 1);
                r->SetProjectionMatrix(m.matrix);
                r->SetupFaceCull(false);
            }
        }
        
        void RenderManager::end() {
            
        }
        
        void RenderManager::doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count) {
            GHL::Render* r = m_resources->GetRender();
            if (r) {
                VertexBuffer* vb = reinterpret_cast<VertexBuffer*>(_buffer);
                Texture* tex = reinterpret_cast<Texture*>(_texture);
                if (vb && tex) {
                    r->SetTexture(tex->Present());
                    r->DrawPrimitivesFromMemory(GHL::PRIMITIVE_TYPE_TRIANGLES,
                                                GHL::VERTEX_TYPE_SIMPLE, vb->GetData(),
                                                _count, 0, _count/3);
                }
            }
        }
        
        const MyGUI::RenderTargetInfo& RenderManager::getInfo() {
            return m_rt_info;
        }

        
        void    RenderManager::drawFrame(float dt) {
            onFrameEvent(dt);
            begin();
            onRenderToTarget(this, true);
            end();
        }
    }
    
    
}
