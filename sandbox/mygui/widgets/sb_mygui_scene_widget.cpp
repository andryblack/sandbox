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
        
        SceneWidget::SceneWidget() : m_texture(0) {
            m_texture_name = get_type_info()->name;
            char buf[128];
            sb::snprintf(buf, 128, "_%p", this);
            m_texture_name += buf;
        }
        
        SceneWidget::~SceneWidget() {
            
        }
        
        void SceneWidget::initialiseOverride()  {
            Base::initialiseOverride();
            if (!m_texture) {
                m_texture = MyGUI::RenderManager::getInstance().createTexture(m_texture_name);
            }
            setRenderItemTexture(m_texture);
        }
        
        void SceneWidget::shutdownOverride() {
            Base::shutdownOverride();
            if (m_texture) {
                MyGUI::RenderManager::getInstance().destroyTexture(m_texture);
            }
        }
        
        void SceneWidget::setScene( const RTScenePtr& scene ) {
            m_scene = scene;
            if (m_scene) {
                Sandbox::RenderTargetPtr rt = m_scene->GetTarget();
                static_cast<Sandbox::mygui::RenderManager&>(MyGUI::RenderManager::getInstance()).wrapRT(m_texture, rt);
                _updateView();
            }
        }
        
    }
}
