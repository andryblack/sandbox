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
    
    RenderTarget::RenderTarget(GHL::RenderTarget* rt) : m_rt(rt){
        GHL::Texture* tex = rt->GetTexture();
        tex->AddRef();
        m_texture = sb::make_shared<Texture>(tex);
    }
    
    RenderTarget::~RenderTarget() {
        m_texture.reset();
        m_rt->Release();
    }
    
}
