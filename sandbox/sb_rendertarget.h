//
//  sb_rendertatget.h
//  caleydoscope
//
//  Created by Andrey Kunitsyn on 7/6/13.
//  Copyright (c) 2013 andryblack. All rights reserved.
//

#ifndef __caleydoscope__sb_rendertatget__
#define __caleydoscope__sb_rendertatget__

#include "sb_texture.h"


namespace GHL {
    struct RenderTarget;
}

namespace Sandbox {
    
    class RenderTarget : public sb::ref_countered_base_not_copyable {
    public:
        explicit RenderTarget(GHL::RenderTarget* rt);
        ~RenderTarget();
        const TexturePtr& GetTexture() const { return m_texture; }
        GHL::RenderTarget* GetNative() { return m_rt; }
    private:
        GHL::RenderTarget*   m_rt;
        TexturePtr  m_texture;
    };
    typedef sb::intrusive_ptr<RenderTarget> RenderTargetPtr;
}

#endif /* defined(__caleydoscope__sb_rendertatget__) */
