//
//  sb_mygui_scene_widget.cpp
//  Sandbox
//
//  Created by Andrey Kunitsyn on 02/02/14.
//
//

#include "sb_mygui_scene_widget.h"
#include "../sb_mygui_render.h"
#include <sbstd/sb_platform.h>

SB_META_DECLARE_OBJECT(Sandbox::mygui::SceneWidget, MyGUI::Widget)

namespace Sandbox {
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(SceneWidget)
        
        SceneWidget::SceneWidget() : m_target(0) {
            m_texture_name = get_type_info()->name;
            char buf[128];
            sb::snprintf(buf, 128, "_%p", this);
            m_texture_name += buf;
        }
        
        SceneWidget::~SceneWidget() {
            delete m_target;
        }
        
        void SceneWidget::initialiseOverride()  {
            Base::initialiseOverride();
        }
        
        void SceneWidget::shutdownOverride() {
            Base::shutdownOverride();
        }
        
        void SceneWidget::setScene( const RTScenePtr& scene ) {
            m_scene = scene;
            if (m_target) {
                delete m_target;
                m_target = 0;
            }
            if (m_scene) {
                Sandbox::RenderTargetPtr rt = m_scene->GetTarget();
                m_target = static_cast<Sandbox::mygui::RenderManager&>(MyGUI::RenderManager::getInstance()).wrapTarget(rt);
                if (m_target) {
                    setRenderItemTexture(m_target->getTexture());
                }
                _updateView();
            }
        }
        
    }
}
