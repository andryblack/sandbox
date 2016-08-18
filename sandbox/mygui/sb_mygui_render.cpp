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
#include "MyGUI_VertexData.h"

SB_META_DECLARE_OBJECT(MyGUI::ITexture, MyGUI::IObject)

namespace Sandbox {
    
    
    namespace mygui {
        
        
                
        RenderTargetImpl::RenderTargetImpl(Graphics* g,Resources* res) : m_graphics(g),m_resources(res) {
        }
        
        void RenderTargetImpl::drawScene(const ScenePtr &scene) {
            Transform2d tr = m_graphics->GetTransform();
            scene->Draw(*m_graphics);
            m_graphics->SetTransform(tr);
        }
        
        
        void RenderTargetImpl::begin() {
            m_graphics->SetBlendMode(BLEND_MODE_ALPHABLEND);
            m_graphics->BeginDrawTriangles(TexturePtr());
        }
        
        void RenderTargetImpl::end() {
            m_graphics->Flush();
        }
        
        void RenderTargetImpl::setSize(int width, int height) {
            
            m_rendertarget_size.set(width,height);

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
                render->Clear(0, 0, 0, 0, 0);
                m_graphics->BeginScene(render, m_target);
                RenderTargetImpl::begin();
            }
        }
        void TextureImpl::setTexture(MyGUI::ITexture* _texture) {
            RenderTargetImpl::setTexture(_texture);
        }
        
        void TextureImpl::addVertex(const MyGUI::Vertex& v) {
            RenderTargetImpl::addVertex(v);
        }
        
        void TextureImpl::end() {
            RenderTargetImpl::end();
            m_graphics->EndScene();
            GHL::Render* render = m_resources->GetRender();
            render->EndScene();
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
        
        RenderTargetImpl::RenderTargetImpl()  {
            
        }
        
        void RenderTargetImpl::setTexture(MyGUI::ITexture *_texture) {
            if (_texture) {
                TextureImpl* tex = static_cast<TextureImpl*>(_texture);
                m_graphics->BeginDrawTriangles(tex->GetTexture());
            } else {
                m_graphics->BeginDrawTriangles(TexturePtr());
            }
        }
        
        void RenderTargetImpl::addVertex(const MyGUI::Vertex &v) {
            m_graphics->AppendVertex(Sandbox::Vector2f(v.x,v.y), Sandbox::Vector2f(v.u,v.v), Color(v.colour));
        }
        
        void RenderTargetImpl::addQuad(const MyGUI::VertexQuad& q) {
            for (int i=0;i<MyGUI::VertexQuad::VertexCount;++i) {
                const MyGUI::Vertex& v(q.vertex[i]);
                m_graphics->AppendVertex(Sandbox::Vector2f(v.x,v.y), Sandbox::Vector2f(v.u,v.v), Color(v.colour));
            }
        }
        
        
        
        RenderManager::RenderManager( Graphics* graphics, Resources* res ) : RenderTargetImpl(graphics,res) {
            m_graphics = graphics;
            setSize(m_resources->GetRender()->GetWidth(),
                    m_resources->GetRender()->GetHeight());
            
            onResizeView(m_rendertarget_size);
        }
        
        ImagePtr RenderManager::getImage(const std::string& name) {
            if (m_context && m_context->GetValue<bool>("get_image")) {
                return m_context->call_self<ImagePtr>("get_image",name);
            }
            return ImagePtr();
        }
        void RenderManager::setContext(const LuaContextPtr& ctx ) {
            m_context = ctx;
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
        
        void RenderManager::wrapTexture( const TexturePtr& texture ) {
            sb_assert(texture);
            const sb::string& name(texture->GetName());
            sb_assert(m_textures.find(name)==m_textures.end());
            TextureImpl* tex = new TextureImpl(name,m_graphics,m_resources);
            m_textures[name] = tex;
            tex->SetTexture(texture);
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
