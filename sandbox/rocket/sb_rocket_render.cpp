//
//  sb_rocket_render.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_rocket_render.h"
#include "sb_resources.h"
#include "sb_graphics.h"
#include "sb_rocket_font.h"

namespace Sandbox {
    
    RocketRenderInterface::RocketRenderInterface(Resources* res) : m_resources(res), m_graphics(0) {
        m_clip_enabled = false;
    }
    
    void RocketRenderInterface::BeginDraw(Graphics& g) {
        m_graphics = &g;
        m_clip_rect = g.GetViewport();
    }
    void RocketRenderInterface::EndDraw(Graphics& g) {
        sb_assert(m_graphics==&g);
        m_graphics = 0;
    }
    
    void RocketRenderInterface::RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation) {
        sb_assert(m_graphics);
        TexturePtr tex = *reinterpret_cast<TexturePtr*>(texture);
        sb::vector<GHL::Vertex> vbuf;
        sb::vector<GHL::UInt16> ibuf;
        vbuf.resize(num_vertices);
        ibuf.resize(num_indices);
        for (size_t i=0;i<vbuf.size();++i) {
            vbuf[i].x = vertices[i].position.x+translation.x;
            vbuf[i].y = vertices[i].position.y+translation.y;
            vbuf[i].z = 0.0f;
            vbuf[i].color[0]=vertices[i].colour.red;
            vbuf[i].color[1]=vertices[i].colour.green;
            vbuf[i].color[2]=vertices[i].colour.blue;
            vbuf[i].color[3]=vertices[i].colour.alpha;
            vbuf[i].tx = vertices[i].tex_coord.x;
            vbuf[i].ty = vertices[i].tex_coord.y;
        }
        for (size_t i=0;i<ibuf.size();++i) {
            ibuf[i]=indices[i];
        }
        m_graphics->DrawBuffer(tex, GHL::PRIMITIVE_TYPE_TRIANGLES, vbuf, ibuf);
    }
    
    void RocketRenderInterface::EnableScissorRegion(bool enable) {
        sb_assert(m_graphics);
        if (enable) {
            m_graphics->SetClipRect(m_clip_rect);
        } else {
            m_graphics->SetClipRect(m_graphics->GetViewport());
        }
        m_clip_enabled = enable;
    }
    
    void RocketRenderInterface::SetScissorRegion(int x, int y, int width, int height) {
        m_clip_rect.x = x;
        m_clip_rect.y = y;
        m_clip_rect.w = width;
        m_clip_rect.h = height;
        if (m_clip_enabled) {
            m_graphics->SetClipRect(m_clip_rect);
        }
    }
    
    bool RocketRenderInterface::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source) {
        TexturePtr tex = m_resources->GetTexture(source.CString(), false);
        if (!tex) return false;
        texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(new TexturePtr(tex));
        texture_dimensions.x = tex->GetOriginalWidth();
        texture_dimensions.y = tex->GetOriginalHeight();
        return true;
    }
    
    bool RocketRenderInterface::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions) {
        return false;
    }
    
    void RocketRenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture) {
        delete reinterpret_cast<TexturePtr*>(texture);
    }
    
    int RocketRenderInterface::RenderString(Rocket::Core::FontFaceHandle* font_face, const Rocket::Core::WString& string, const Rocket::Core::Vector2f& position, const Rocket::Core::Colourb& colour) const {
        if (!font_face) return 0;
        RocketFontFaceHandle* rfont = reinterpret_cast<RocketFontFaceHandle*>(font_face);
        return rfont->Draw(*m_graphics,string,position,colour);
    }

}