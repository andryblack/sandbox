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
#include "sb_mygui_widget_render.h"
#include "MyGUI_LayoutManager.h"
#include "MyGUI_VertexData.h"
#include <sbstd/sb_platform.h>

SB_META_DECLARE_OBJECT(MyGUI::ITexture, MyGUI::IObject)

namespace Sandbox {
    
    
    namespace mygui {
        
        
                
        RenderTargetImpl::RenderTargetImpl(Graphics* g,Resources* res,RenderTargetPtr rt) : m_graphics(g),m_resources(res),m_target(rt),m_texture(0) {
            if (m_target) {
                m_target->GetTexture()->SetFiltered(true);
                m_rendertarget_size.width = m_target->GetWidth();
                m_rendertarget_size.height = m_target->GetHeight();
                m_texture = new TextureImpl("",m_target->GetTexture());
                begin();
                end();
            }
        }
        
        void RenderTargetImpl::resize( const MyGUI::IntSize& size) {
            if (m_rendertarget_size == size)
                return;
            float scale = RenderManager::getInstance().getRTScale();
            RenderTargetPtr prev = m_target;
            float full_scale = m_resources->GetScale() * scale;
            if (m_target ) {
                if (m_target->GetWidth() >= size.width &&
                    m_target->GetHeight() >= size.height)
                    return;
            }
            m_target = m_resources->CreateRenderTarget(size.width, size.height,
                                                       full_scale, true, false);
            if (!m_target) {
                m_target = prev;
            }
            if (!m_target)
                return;
            m_target->GetTexture()->SetFiltered(true);
            begin();
            end();
            if (!m_texture) {
                m_texture = new TextureImpl("",m_target->GetTexture());
            } else {
                m_texture->SetTexture(m_target->GetTexture());
            }
            setSize(size.width, size.height);
        }
        RenderTargetImpl::~RenderTargetImpl() {
            delete m_texture;
        }
        
        void RenderTargetImpl::drawScene(const ScenePtr &scene) {
            Transform2d tr = m_graphics->GetTransform();
            scene->Draw(*m_graphics);
            m_graphics->SetTransform(tr);
        }
        
        
        void RenderTargetImpl::begin() {
            if (m_target) {
                GHL::RenderTarget* rt = m_target->GetNative();
                GHL::Render* render = m_resources->GetRender();
                render->BeginScene(rt);
                render->Clear(0, 0, 0, 0, 0);
                m_graphics->BeginScene(render, m_target);
                m_graphics->SetBlendMode(BLEND_MODE_ALPHABLEND);
                m_graphics->BeginDrawTriangles(TexturePtr());
            }
        }
        
        void RenderTargetImpl::end() {
            if (m_target) {
                m_graphics->Flush();
                m_graphics->EndScene();
                GHL::Render* render = m_resources->GetRender();
                render->EndScene();
            }
        }
        
        void RenderTargetImpl::setSize(int width, int height) {
            
            m_rendertarget_size.set(width,height);

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
            const MyGUI::Vertex& lt(q.vertex[MyGUI::VertexQuad::CornerLT]);
            const MyGUI::Vertex& rt(q.vertex[MyGUI::VertexQuad::CornerRT]);
            const MyGUI::Vertex& lb(q.vertex[MyGUI::VertexQuad::CornerLB]);
            const MyGUI::Vertex& rb(q.vertex[MyGUI::VertexQuad::CornerRB]);
            m_graphics->AppendQuad(
                                   Sandbox::Vector2f(lt.x,lt.y),Sandbox::Vector2f(lt.u,lt.v),Color(lt.colour),
                                   Sandbox::Vector2f(rt.x,rt.y),Sandbox::Vector2f(rt.u,rt.v),Color(rt.colour),
                                   Sandbox::Vector2f(lb.x,lb.y),Sandbox::Vector2f(lb.u,lb.v),Color(lb.colour),
                                   Sandbox::Vector2f(rb.x,rb.y),Sandbox::Vector2f(rb.u,rb.v),Color(rb.colour));
        }
        
        
        
        RenderManager::RenderManager( Graphics* graphics, Resources* res ) : RenderTargetImpl(graphics,res,RenderTargetPtr()) , m_scale(1.0f) {
            m_graphics = graphics;
        }
        
        void RenderManager::reshape(float w,float h) {
            setSize(w,h);
            onResizeView(m_rendertarget_size);
        }
        
        ImagePtr RenderManager::getImage(const std::string& name) {
            if (m_context && m_context->GetValue<bool>("get_image")) {
                return m_context->call_self<ImagePtr>("get_image",name);
            }
            return ImagePtr();
        }
        
        FontPtr RenderManager::getFont( const sb::string& _name ) {
            if (m_context && m_context->GetValue<bool>("get_font")) {
                return m_context->call_self<FontPtr>("get_font",_name);
            }
            return FontPtr();
        }
        
        void RenderManager::setContext(const LuaContextPtr& ctx ) {
            m_context = ctx;
        }
        
        float RenderManager::getDisplayScale() const {
            return m_resources->GetScale();
        }
        
        TextureImpl::TextureImpl( const sb::string& name,
                                 const TexturePtr& tex) : m_name(name),m_texture(tex) {
            if (name.empty()) {
                if (m_texture && !m_texture->GetName().empty()) {
                    m_name = m_texture->GetName();
                } else {
                    char buff[128];
                    sb::snprintf(buff, 128, "generated_%p",this);
                    m_name = buff;
                    if (m_texture) m_texture->SetName(m_name);
                }
            }
        }
        
        
    
        
        void TextureImpl::destroy() {
            m_texture.reset();
        }
        
        int TextureImpl::getWidth() {
            return m_texture ? m_texture->GetWidth() : 0;
        }
        int TextureImpl::getHeight() {
            return m_texture ? m_texture->GetHeight() : 0;
        }

        
        MyGUI::ITexture* RenderManager::loadTexture(const std::string& _file_name) {
            sb_assert(m_textures.find(_file_name)==m_textures.end());
            TexturePtr texture = m_resources->GetTexture(_file_name.c_str(), false);
            if (!texture) {
                return 0;
            }
            texture->SetFiltered(true);
            TextureImpl* tex = new TextureImpl(_file_name,texture);
            m_textures[_file_name] = tex;
            return tex;
        }
        
        MyGUI::ITexture* RenderManager::wrapTexture( const TexturePtr& texture ) {
            sb_assert(texture);
            const sb::string& name(texture->GetName());
            sb_assert(m_textures.find(name)==m_textures.end());
            TextureImpl* tex = new TextureImpl(name,texture);
            m_textures[name] = tex;
            tex->SetTexture(texture);
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
        
        void RenderManager::setRTScale(float s) {
            m_scale = s;
        }
        
        RenderTargetImpl* RenderManager::createTarget(MyGUI::IntSize size) {
            if (size.width <= 0 || size.height <= 0)
                return 0;
            RenderTargetPtr target = m_resources->CreateRenderTarget(size.width, size.height,
                                                                    m_scale*m_resources->GetScale(),
                                                                     true, false);
            if (!target) return 0;
            RenderTargetImpl* impl = new RenderTargetImpl(m_graphics,m_resources,target);
            return impl;
        }
        
        RenderTargetImpl* RenderManager::wrapTarget(const RenderTargetPtr& target ) {
            if (!target)
                return 0;
            return new RenderTargetImpl(m_graphics,m_resources,target);
        }
                
        const MyGUI::IntSize& RenderManager::getViewSize() const {
            return m_rendertarget_size;
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
            m_graphics->SetBlendMode(BLEND_MODE_ALPHABLEND);
            m_graphics->BeginDrawTriangles(TexturePtr());

            onRenderToTarget(this, false);
            m_graphics->Flush();

        }
        
        
        WidgetRenderPtr RenderManager::renderLayout( const MyGUI::IntSize& size, const sb::string& layout ) {
            WidgetRender* render = new WidgetRender(size);
            MyGUI::LayoutManager::getInstance().loadLayout(layout,"",render);
            return WidgetRenderPtr(render);
        }
        
    }
    
    
}
