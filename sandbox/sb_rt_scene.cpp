//
//  sb_rt_scene.cpp
//  caleydoscope
//
//  Created by Andrey Kunitsyn on 7/6/13.
//  Copyright (c) 2013 andryblack. All rights reserved.
//

#include "sb_rt_scene.h"
#include "sb_graphics.h"
#include <ghl_render.h>

SB_META_DECLARE_OBJECT(Sandbox::RTScene, Sandbox::Scene)

namespace Sandbox {
    
    RTScene::RTScene(const RenderTargetPtr& rt ) : m_rt(rt) {
        
    }
    
    void RTScene::Draw(GHL::Render *r,Graphics& g) {
        r->BeginScene(m_rt->GetNative());
        g.BeginScene(r);
        Scene::Draw(g);
        g.EndScene();
        r->EndScene();
    }
    
}