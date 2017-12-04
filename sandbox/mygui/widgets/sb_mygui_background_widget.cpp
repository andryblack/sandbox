#include "sb_mygui_background_widget.h"
#include "../sb_mygui_render.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::BackgroundWidget, Sandbox::mygui::SceneObjectWidget)

namespace Sandbox {
    
    namespace mygui {
        
        class BackgroundImpl : public Background {
        public:
            void Draw( Graphics& g ) const SB_OVERRIDE {
                Transform2d tr = g.GetTransform();
                if (GetFullScreen()) {
                    g.SetTransform(tr.translated(-tr.v));
                }
                Background::Draw(g);
                g.SetTransform(tr);
            }
        };
        
        MYGUI_IMPL_TYPE_NAME(BackgroundWidget)
        
        BackgroundWidget::BackgroundWidget() : m_background(new BackgroundImpl()) {
            
        }
        void BackgroundWidget::setBackground(const char* name) {
            m_background->Load(name, RenderManager::getInstance().resources());
        }
        
        void BackgroundWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "Background") {
                setBackground(_value.c_str());
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
