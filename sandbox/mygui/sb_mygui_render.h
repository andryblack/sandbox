//
//  sb_mygui_render.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 25/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_mygui_render__
#define __sr_osx__sb_mygui_render__

#include "MyGUI_RenderManager.h"
#include "MyGUI_IRenderTarget.h"
#include <sbstd/sb_list.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_string.h>

#include "sb_scene.h"
#include <ghl_render.h>
#include "sb_data.h"

namespace GHL {
    struct Render;
}

namespace Sandbox {
    
    class Resources;
    class Graphics;
    class Texture;
    typedef sb::intrusive_ptr<Texture> TexturePtr;
    
    class RenderTarget;
    typedef sb::intrusive_ptr<RenderTarget> RenderTargetPtr;
    
    namespace mygui {
        
        class VertexBufferImpl : public MyGUI::IVertexBuffer {
        public:
            VertexBufferImpl() {

            }
            virtual void setVertexCount(size_t _value) {
                m_data.resize(_value);
            }
            virtual size_t getVertexCount() {
                return m_data.size();
            }
            
            virtual MyGUI::Vertex* lock() {
                return reinterpret_cast<MyGUI::Vertex*>(m_data.data());
            }
            virtual void unlock() {
                
            }
            const void* GetData() {
                return m_data.data();
            }
            
            sb::vector<GHL::Vertex>& data() { return m_data; }
        private:
            sb::vector<GHL::Vertex>  m_data;
        };

        
        class TextureImpl;
        
        class RenderTargetImpl : public MyGUI::IRenderTarget {
        public:
            RenderTargetImpl();
            virtual void begin();
            virtual void end();
            
            virtual void doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count);
            
            virtual const MyGUI::RenderTargetInfo& getInfo();
            
            void    drawScene(const ScenePtr& scene);
            
            void    startRenderMask( MyGUI::ITexture* tex_mask );
            void    startRenderObject();
            void    endRenderMask();
            void    endRenderObject();
            
            GHL::Render* render() { return m_render; }
            Resources* resources() { return m_resources; }
            Graphics* graphics() { return m_graphics; }
        protected:
            RenderTargetImpl( Graphics* graphics, Resources* resources );
            Graphics*       m_graphics;
            Resources*      m_resources;
            MyGUI::RenderTargetInfo m_rt_info;
            MyGUI::IntSize  m_rendertarget_size;
            void setSize(int width,int height);
            GHL::Render*    m_render;
            TextureImpl*    m_draw_mask;
        };
        
        class TextureImpl : public MyGUI::ITexture, public RenderTargetImpl {
        public:
            TextureImpl( const sb::string& name, Graphics* graphics,Resources* res);
            TextureImpl( const sb::string& name, Graphics* graphics,Resources* res,
                        RenderTargetPtr rt );
            
            void WrapRT( RenderTargetPtr rt );
            
            virtual void begin();
            virtual void doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count);
            void end();
            
            virtual MyGUI::IRenderTarget* getRenderTarget();
            virtual const std::string& getName() const { return m_name; }
            virtual void createManual(int _width, int _height, MyGUI::TextureUsage _usage, MyGUI::PixelFormat _format);
            virtual void loadFromFile(const std::string& _filename);
            virtual void saveToFile(const std::string& _filename) {
                
            }
            virtual void destroy();

            virtual void* lock(MyGUI::TextureUsage _access);
            virtual void unlock();
            virtual bool isLocked() {
                return m_image!=0;
            }
            virtual int getWidth();
            virtual int getHeight();
            
            virtual MyGUI::PixelFormat getFormat();
            virtual MyGUI::TextureUsage getUsage();
            virtual size_t getNumElemBytes();
            const GHL::Texture* Present();
        private:
            sb::string  m_name;
            TexturePtr  m_texture;
            RenderTargetPtr m_target;
            GHL::Image* m_image;
        };
        
        class RenderManager : public MyGUI::RenderManager, public RenderTargetImpl {
        public:
            explicit RenderManager( Graphics* graphics, Resources* resources );
            
            
            static RenderManager& getInstance() { return static_cast<RenderManager&>(MyGUI::RenderManager::getInstance()); }
            /** Create vertex buffer.
             This method should create vertex buffer with triangles list type,
             each vertex have position, colour, texture coordinates.
             */
            virtual MyGUI::IVertexBuffer* createVertexBuffer();
            /** Destroy vertex buffer */
            virtual void destroyVertexBuffer(MyGUI::IVertexBuffer* _buffer);
            
            /** Create empty texture instance */
            virtual MyGUI::ITexture* createTexture(const std::string& _name);
            /** Destroy texture */
            virtual void destroyTexture(MyGUI::ITexture* _texture);
            /** Get texture by name */
            virtual MyGUI::ITexture* getTexture(const std::string& _name);
            
            //FIXME возможно перенести в структуру о рендер таргете
            virtual const MyGUI::IntSize& getViewSize() const;
            
            /** Get current vertex colour type */
            virtual MyGUI::VertexColourType getVertexFormat();
            
            /** Check if texture format supported by hardware */
            virtual bool isFormatSupported(MyGUI::PixelFormat _format, MyGUI::TextureUsage _usage);
            
#if MYGUI_DEBUG_MODE == 1
            /** Check if texture is valid */
            virtual bool checkTexture(MyGUI::ITexture* _texture);
#endif
            
            
            void    timerFrame(float dt);
            void    drawFrame();
            
            virtual float getDisplayScale() const;
            
            
            
            void wrapRT( MyGUI::ITexture* texture, const RenderTargetPtr& rt );
        private:
            sb::map<sb::string,TextureImpl*>  m_textures;
            
        };
        
    }
    
}

#endif /* defined(__sr_osx__sb_mygui_render__) */
