#include "sb_mygui_selectable_widget.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::SelectableWidget, MyGUI::Widget)


namespace Sandbox {
    namespace mygui {
        
        SelectableWidget::SelectableWidget() : m_selected(false) {
            
        }
        
        SelectableWidget::~SelectableWidget() {
            
        }
        
        void SelectableWidget::setSelected(bool selected) {
            m_selected = selected;
            updateState();
        }
        
        void SelectableWidget::baseUpdateEnable() {
            updateState();
        }
       
        void SelectableWidget::updateState() {
            if (getInheritedEnabled()) {
                if (m_selected) {
                    _setWidgetState("selected");
                } else {
                    _setWidgetState("normal");
                }
            } else {
                if (m_selected) {
                    if (!_setWidgetState("disabled_selected"))
                        _setWidgetState("selected");
                } else {
                    if (!_setWidgetState("disabled")) {
                        _setWidgetState("normal");
                    }
                }
            }
        }
        
        void SelectableWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "Selected") {
                setSelected(MyGUI::utility::parseValue<bool>(_value));
            }
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
    }
}
