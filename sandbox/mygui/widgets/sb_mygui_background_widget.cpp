#include "sb_mygui_background_widget.h"
#include "../sb_mygui_render.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::BackgroundWidget, Sandbox::mygui::SceneObjectWidget)

namespace Sandbox {
    
    namespace mygui {
        
        BackgroundWidget::BackgroundWidget() : m_background(new Background()) {
            
        }
        
        void BackgroundWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "Background") {
                m_background->Load(_value.c_str(),RenderManager::getInstance().resources());
            } else if (_key == "Fullscreen") {
                m_background->SetFullScreen(MyGUI::utility::parseBool(_value));
            } else if (_key == "Filtered") {
                m_background->SetFiltered(MyGUI::utility::parseBool(_value));
            } else if (_key == "KeepAspect") {
                m_background->SetKeepAspect(MyGUI::utility::parseBool(_value));
            }
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
        
        void BackgroundWidget::initialiseOverride() {
            setObject(m_background);
        }
        
        void BackgroundWidget::shutdownOverride() {
            m_background->Clear();
        }
    }
    
}
