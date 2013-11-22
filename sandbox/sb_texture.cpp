//
//  sb_texture.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 5/5/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_texture.h"
#include "sb_resources.h"
#include <ghl_texture.h>

namespace Sandbox {
    
    const size_t TEXTURE_TTL = 16;
    
    
    Texture::Texture(const sb::string& file, bool premul, GHL::UInt32 w, GHL::UInt32 h) :
    m_texture(0),m_file(file),m_original_w(w),m_original_h(h),m_live_ticks(0),m_filtered(false),m_tiled(false){
        m_need_premultiply = premul;
    }
    
    Texture::Texture(GHL::Texture* tex, GHL::UInt32 w, GHL::UInt32 h) :
    m_texture(tex),m_file(),m_original_w(w),m_original_h(h),m_live_ticks(0),m_filtered(false),m_tiled(false){
        m_need_premultiply = false;
        if (m_texture) {
            if (m_original_w==0) m_original_w = m_texture->GetWidth();
            if (m_original_h==0) m_original_h = m_texture->GetHeight();
        }
    }
    
    Texture::~Texture() {
        if (m_texture) m_texture->Release();
    }
    
    const GHL::Texture* Texture::Present(Resources* resources) const {
        if (!m_texture) {
            m_texture = resources->LoadTexture( m_file , m_need_premultiply );
            m_texture->SetMinFilter(m_filtered?GHL::TEX_FILTER_LINEAR:GHL::TEX_FILTER_NEAR);
            m_texture->SetMagFilter(m_filtered?GHL::TEX_FILTER_LINEAR:GHL::TEX_FILTER_NEAR);
            m_texture->SetWrapModeU(m_tiled?GHL::TEX_WRAP_REPEAT:GHL::TEX_WRAP_CLAMP);
            m_texture->SetWrapModeV(m_tiled?GHL::TEX_WRAP_REPEAT:GHL::TEX_WRAP_CLAMP);
        }
        m_live_ticks = resources->GetLiveTicks() + TEXTURE_TTL;
        return m_texture;
    }
    
    void Texture::SetFiltered(bool f) {
        if (m_texture) {
            m_texture->SetMinFilter(f?GHL::TEX_FILTER_LINEAR:GHL::TEX_FILTER_NEAR);
            m_texture->SetMagFilter(f?GHL::TEX_FILTER_LINEAR:GHL::TEX_FILTER_NEAR);
        }
        m_filtered = f;
    }
    
    void Texture::SetTiled(bool t) {
        if (m_texture) {
            m_texture->SetWrapModeU(t?GHL::TEX_WRAP_REPEAT:GHL::TEX_WRAP_CLAMP);
            m_texture->SetWrapModeV(t?GHL::TEX_WRAP_REPEAT:GHL::TEX_WRAP_CLAMP);
        }
        m_tiled = t;
    }
    
    size_t Texture::Release() {
        if (m_file.empty()) return 0;
        size_t res = 0;
        if (m_texture) {
            res = m_texture->GetMemoryUsage();
            m_texture->Release();
        }
        m_texture = 0;
        m_live_ticks = 0;
        return res;
    }
    
    size_t Texture::GetMemoryUsage() const {
        if (m_texture) return m_texture->GetMemoryUsage();
        return 0;
    }
        
}
