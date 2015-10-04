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
        
        SceneWidget::SceneWidget() : m_texture(0) {
            
        }
        
        SceneWidget::~SceneWidget() {
            
        }
        
        void SceneWidget::initialiseOverride()  {
            if (m_texture) {
                setRenderItemTexture(m_texture);
            }
        }
        
        void SceneWidget::setScene( const RTScenePtr& scene ) {
            m_scene = scene;
            if (m_scene) {
                if (m_texture) {
                    MyGUI::RenderManager::getInstance().destroyTexture(m_texture);
                }
                Sandbox::RenderTargetPtr rt = m_scene->GetTarget();
                
                char buf[128];
                snprintf(buf, 128, "scene_rt_%p_%p",this,rt.get());
                sb::string texture_name =  buf;
                
                m_texture = static_cast<Sandbox::mygui::RenderManager&>(MyGUI::RenderManager::getInstance()).wrapRT(texture_name, rt);
                
                setRenderItemTexture(m_texture);
            }
        }
        
    }
}
