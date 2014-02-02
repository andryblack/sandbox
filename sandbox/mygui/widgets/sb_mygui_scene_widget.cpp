//
//  sb_mygui_scene_widget.cpp
//  Sandbox
//
//  Created by Andrey Kunitsyn on 02/02/14.
//
//

#include "sb_mygui_scene_widget.h"
#include "../sb_mygui_render.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::SceneWidget, MyGUI::Widget)

namespace Sandbox {
    namespace mygui {
        
        SceneWidget::SceneWidget() {
            
        }
        
        SceneWidget::~SceneWidget() {
            
        }
        
        void SceneWidget::doRenderToTarget(MyGUI::IRenderTarget* rt) {
            if (m_scene) {
                RenderTargetImpl* impl = static_cast<RenderTargetImpl*>(rt);
                impl->drawScene(m_scene);
            }
        }
        
    }
}
