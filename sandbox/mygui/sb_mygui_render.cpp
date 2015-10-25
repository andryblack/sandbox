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
#include "../sb_graphics.h"
#include <ghl_image.h>
#include <ghl_texture.h>
#include "sb_transform2d.h"


namespace Sandbox {
    
    
    namespace mygui {
        
        
                
        RenderTargetImpl::RenderTargetImpl(Graphics* g,Resources* res) : m_graphics(g),m_resources(res),m_render(0) {
            
        }
        
        void RenderTargetImpl::drawScene(const ScenePtr &scene) {
            if (m_render) {
                m_graphics->EndNative(m_render);
                Transform2d tr = m_graphics->GetTransform();
            
                //m_graphics->SetTransform(tr.translated(-m_rt_info.leftOffset, -m_rt_info.topOffset));
                scene->Draw(*m_graphics);
                m_graphics->SetTransform(tr);
                m_render = m_graphics->BeginNative();
            }
        }
        
        const MyGUI::RenderTargetInfo& RenderTargetImpl::getInfo() {
            return m_rt_info;
        }
        
        void RenderTargetImpl::begin() {
            m_graphics->SetBlendMode(BLEND_MODE_ALPHABLEND);
            m_render = m_graphics->BeginNative();
            if (m_render) {
                m_render->SetupBlend(true,GHL::BLEND_FACTOR_ONE,GHL::BLEND_FACTOR_SRC_ALPHA_INV);
                m_render->SetupFaceCull(false);
            }
        }
        
        void RenderTargetImpl::end() {
            if (m_render) {
                m_render->SetupFaceCull(true);
                m_graphics->EndNative(m_render);
                m_render = 0;
            }
        }
        
        void RenderTargetImpl::setSize(int width, int height) {
            
            m_rendertarget_size.set(width,height);
            m_rt_info.maximumDepth = 1;
            m_rt_info.hOffset = 0.5f;//-m_view_size.width/2;
            m_rt_info.vOffset = 0.5f;//-m_view_size.height/2;
            m_rt_info.aspectCoef = float(height) / float(width);
            m_rt_info.pixScaleX = 0.5f;//0.5f;
            m_rt_info.pixScaleY = -0.5f;//-0.5f;

        }

        
        TextureImpl::TextureImpl( const sb::string& name, Graphics* graphics,Resources* res) : RenderTargetImpl(graphics,res),m_name(name),m_image(0) {
                
            }
            
        TextureImpl::TextureImpl( const sb::string& name, Graphics* graphics,Resources* res,
                   RenderTargetPtr rt ) : RenderTargetImpl(graphics,res),m_name(name),m_image(0) {
            m_target = rt;
            m_texture = m_target->GetTexture();
            int _width = m_texture->GetWidth();
            int _height = m_texture->GetHeight();
            setSize(_width, _height);
        }
        
        void TextureImpl::WrapRT( RenderTargetPtr rt ) {
            m_target = rt;
            m_texture = m_target->GetTexture();
            int _width = m_texture->GetWidth();
            int _height = m_texture->GetHeight();
            setSize(_width, _height);
        }
            
        void TextureImpl::begin() {
            if (m_target) {
                GHL::RenderTarget* rt = m_target->GetNative();
                GHL::Render* render = m_resources->GetRender();
                render->BeginScene(rt);
                m_graphics->BeginScene(render,true);
                m_graphics->Clear(Color(0,0,0,0), 0);
                RenderTargetImpl::begin();
            }
        }
        void TextureImpl::doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count) {
            RenderTargetImpl::doRender(_buffer, _texture, _count);
        }
        void TextureImpl::end() {
            GHL::Render* render = m_render;
            RenderTargetImpl::end();
            m_graphics->EndScene();
            if (render) {
                render->EndScene();
            }
        }
            
        MyGUI::IRenderTarget* TextureImpl::getRenderTarget()
        {
            if (m_target)
                return this;
            return 0;
        }
            
            
            
        void TextureImpl::createManual(int _width, int _height, MyGUI::TextureUsage _usage, MyGUI::PixelFormat _format) {
            if (_usage == MyGUI::TextureUsage::RenderTarget) {
                m_target = m_resources->CreateRenderTarget(_width,_height,m_resources->GetScale(),true,false);
                m_texture = m_target->GetTexture();
                setSize(_width, _height);
            } else {
                if (_format == MyGUI::PixelFormat::R8G8B8A8) {
                    m_texture = m_resources->CreateTexture(_width, _height, m_resources->GetScale(), true, 0);
                }
            }
            if (m_texture) {
                m_texture->SetFiltered(true);
            }
        }
            
        void TextureImpl::loadFromFile(const std::string& _filename) {
            m_texture = m_resources->GetTexture(_filename.c_str(), false);
            if (m_texture) {
                m_texture->SetFiltered(true);
            }
        }
            
        
          
        void TextureImpl::destroy() {
            m_texture.reset();
        }
            
        void* TextureImpl::lock(MyGUI::TextureUsage _access) {
            if (!m_texture) return 0;
            m_image = GHL_CreateImage(m_texture->GetRealWidth(), m_texture->GetRealHeight(), GHL::IMAGE_FORMAT_RGBA);
            return m_image->GetDataPtr();
        }
            
        void TextureImpl::unlock() {
            if (m_image && m_texture) {
                GHL::Texture* tex = m_texture->Present(m_resources);
                {
                    //m_image->PremultiplyAlpha();
                }
                tex->SetData(0, 0, m_image,0);
                m_image->Release();
                m_image = 0;
                tex->DiscardInternal();
            }
        }
        
        int TextureImpl::getWidth() {
            return m_texture ? m_texture->GetWidth() : 0;
        }
        int TextureImpl::getHeight() {
            return m_texture ? m_texture->GetHeight() : 0;
        }
            
        MyGUI::PixelFormat TextureImpl::getFormat() {
            return MyGUI::PixelFormat::R8G8B8A8;
        }
        MyGUI::TextureUsage TextureImpl::getUsage() {
            return MyGUI::TextureUsage::Default;
        }
        size_t TextureImpl::getNumElemBytes() {
            return 0;
        }
        const GHL::Texture* TextureImpl::Present() {
            return m_texture ? m_texture->Present(m_resources) : reinterpret_cast<GHL::Texture*>((void*)(0));
        }
        
        RenderTargetImpl::RenderTargetImpl() : m_draw_mask(0) {
            
        }
        
        
        void RenderTargetImpl::doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count) {
            endRenderObject();
            if (m_render) {
                endRenderMask();
                VertexBufferImpl* vb = reinterpret_cast<VertexBufferImpl*>(_buffer);
                TextureImpl* tex = reinterpret_cast<TextureImpl*>(_texture);
                if (vb && tex) {
                    m_render->SetTexture(tex->Present());
                    m_render->DrawPrimitivesFromMemory(GHL::PRIMITIVE_TYPE_TRIANGLES,
                                                GHL::VERTEX_TYPE_SIMPLE, vb->GetData(),
                                                _count, 0, _count/3);
                }
            }
        }

        void    RenderTargetImpl::startRenderMask( MyGUI::ITexture* tex_mask ) {
            TextureImpl* tex = reinterpret_cast<TextureImpl*>(tex_mask);
            if (m_draw_mask == tex) {
                return;
            }
            m_draw_mask = tex;
            if (m_render) {
                m_render->SetTexture(m_draw_mask->Present(),1);
                m_render->SetupTextureStageColorOp(GHL::TEX_OP_MODULATE, GHL::TEX_ARG_CURRENT, GHL::TEX_ARG_TEXTURE, 1);
                m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_MODULATE, GHL::TEX_ARG_CURRENT, GHL::TEX_ARG_TEXTURE, 1);
            }
        }
        
        void    RenderTargetImpl::endRenderMask() {
            if (!m_draw_mask) {
                return;
            }
            if (m_render) {
                m_render->SetShader(0);
                m_render->SetTexture(0,1);
                m_render->SetupTextureStageColorOp(GHL::TEX_OP_DISABLE, GHL::TEX_ARG_CURRENT, GHL::TEX_ARG_TEXTURE, 1);
                m_render->SetupTextureStageAlphaOp(GHL::TEX_OP_DISABLE, GHL::TEX_ARG_CURRENT, GHL::TEX_ARG_TEXTURE, 1);
            }
            m_draw_mask = 0;
        }
        
        void    RenderTargetImpl::endRenderObject() {
            if (m_render) {
                return;
            }
            RenderTargetImpl::begin();
        }
        
        void    RenderTargetImpl::startRenderObject() {
            endRenderMask();
            if (m_render) {
                RenderTargetImpl::end();
            }
        }
        
        
        RenderManager::RenderManager( Graphics* graphics, Resources* res ) : RenderTargetImpl(graphics,res) {
            m_graphics = graphics;
            setSize(m_resources->GetRender()->GetWidth(),
                    m_resources->GetRender()->GetHeight());
            
            onResizeView(m_rendertarget_size);
        }
        
        MyGUI::IVertexBuffer* RenderManager::createVertexBuffer() {
            return new VertexBufferImpl();
        }
        
        void RenderManager::destroyVertexBuffer(MyGUI::IVertexBuffer* _buffer) {
            delete _buffer;
        }
        
        float RenderManager::getDisplayScale() const {
            return m_resources->GetScale();
        }
        
        MyGUI::ITexture* RenderManager::createTexture(const std::string& _name) {
            sb_assert(m_textures.find(_name)==m_textures.end());
            
            TextureImpl* tex = new TextureImpl(_name,m_graphics,m_resources);
            m_textures[_name] = tex;
            return tex;
        }
        
        void RenderManager::destroyTexture(MyGUI::ITexture* _texture) {
            if (!_texture) return;
            TextureImpl* tex = reinterpret_cast<TextureImpl*>(_texture);
            sb::map<sb::string, TextureImpl*>::iterator it = m_textures.find(tex->getName());
            sb_assert(it!=m_textures.end());
            m_textures.erase(it);
            delete tex;
        }
        
        MyGUI::ITexture* RenderManager::getTexture(const std::string& _name) {
            sb::map<sb::string, TextureImpl*>::iterator it = m_textures.find(_name);
            if (it!=m_textures.end()) {
                return it->second;
            }
            return 0;
        }
        
        void RenderManager::wrapRT(MyGUI::ITexture* texture,const RenderTargetPtr& rt ) {
            if (!texture) return;
            TextureImpl* tex = static_cast<TextureImpl*>(texture);
            tex->WrapRT(rt);
        }
        
        const MyGUI::IntSize& RenderManager::getViewSize() const {
            return m_rendertarget_size;
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
        
        
        void RenderManager::timerFrame(float dt) {
            onFrameEvent(dt);
        }
        
        void    RenderManager::drawFrame() {
            begin();
            onRenderToTarget(this, true);
            end();
        }
        
        
    }
    
    
}
