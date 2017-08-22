//
//  sb_rendertatget.cpp
//  caleydoscope
//
//  Created by Andrey Kunitsyn on 7/6/13.
//  Copyright (c) 2013 andryblack. All rights reserved.
//

#include "sb_rendertarget.h"
#include <ghl_render_target.h>
#include <ghl_texture.h>

namespace Sandbox {
    
    RenderTarget::RenderTarget(GHL::RenderTarget* rt,int w, int h, float scale) : m_rt(rt){
        GHL::Texture* tex = rt->GetTexture();
        tex->AddRef();
        m_texture.reset( new Texture(tex,1.0f/scale,w,h) );
    }
    
    GHL::UInt32 RenderTarget::GetWidth() const {
        return m_rt ? m_rt->GetWidth() / GetScale() : 0;
    }
    GHL::UInt32 RenderTarget::GetHeight() const {
        return m_rt ? m_rt->GetHeight() / GetScale() : 0;
    }
    
    float RenderTarget::GetScale() const {
        if (m_texture) return 1.0f / m_texture->GetScale();
        return 1.0f;
    }
    
    RenderTarget::~RenderTarget() {
        Discard();
    }
    
    void RenderTarget::Discard() {
        m_texture.reset();
        if (m_rt) {
            m_rt->Release();
            m_rt = 0;
        }
    }
    
}
