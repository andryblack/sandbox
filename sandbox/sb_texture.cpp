//
//  sb_texture.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 5/5/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_texture.h"
#include "sb_resources.h"
#include "sb_bitmask.h"
#include <ghl_texture.h>

namespace Sandbox {
    
    const size_t TEXTURE_TTL = 16;
    
    
    Texture::Texture(const sb::string& file, float scale,bool premul, GHL::UInt32 w, GHL::UInt32 h) :
    m_texture(0),m_file(file),m_original_w(w),
    m_original_h(h),m_width(w),m_height(h),m_live_ticks(0),m_filtered(false),m_tiled(false),m_scale(scale){
        m_need_premultiply = premul;
    }
    
    Texture::Texture(GHL::Texture* tex, float scale, GHL::UInt32 w, GHL::UInt32 h) :
    m_texture(tex),m_file(),m_original_w(w),m_original_h(h),m_live_ticks(0),m_filtered(false),m_tiled(false),m_scale(scale){
        m_need_premultiply = false;
        if (m_texture) {
            m_width = m_texture->GetWidth();
            m_height = m_texture->GetHeight();
            if (m_original_w==0) m_original_w = m_width;
            if (m_original_h==0) m_original_h = m_height;
        }
    }
    
    Texture::~Texture() {
        if (m_texture) m_texture->Release();
    }
    
    void Texture::SetTextureSize(GHL::UInt32 tw,GHL::UInt32 th) {
        m_width = tw;
        m_height = th;
    }
    
    
    
    GHL::Texture* Texture::Present(Resources* resources) {
        if (!m_texture) {
            bool variant = false;
            m_texture = resources->ManagedLoadTexture( m_file ,variant, m_need_premultiply );
            if (m_texture) {
                m_texture->SetMinFilter(m_filtered?GHL::TEX_FILTER_LINEAR:GHL::TEX_FILTER_NEAR);
                m_texture->SetMagFilter(m_filtered?GHL::TEX_FILTER_LINEAR:GHL::TEX_FILTER_NEAR);
                m_texture->SetWrapModeU(m_tiled?GHL::TEX_WRAP_REPEAT:GHL::TEX_WRAP_CLAMP);
                m_texture->SetWrapModeV(m_tiled?GHL::TEX_WRAP_REPEAT:GHL::TEX_WRAP_CLAMP);
            }
        }
        m_live_ticks = resources->GetLiveTicks() + TEXTURE_TTL;
        return m_texture;
    }
    BitmaskPtr Texture::GetBitmask(Resources* resources) {
        if (!m_bitmask && !m_file.empty()) {
            m_bitmask = resources->LoadBitmask(m_file);
        }
        return m_bitmask;
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
