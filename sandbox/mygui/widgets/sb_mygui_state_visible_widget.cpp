#include "sb_mygui_state_visible_widget.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::StateVisibleWidget, MyGUI::Widget)

namespace Sandbox {
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(StateVisibleWidget)
        
        StateVisibleWidget::StateVisibleWidget() {
            
        }
        
        StateVisibleWidget::~StateVisibleWidget() {
            
        }
        
        void StateVisibleWidget::setVisibleStates(const sb::string& states) {
            sb::string value;
            std::istringstream stream(states);
            while (!stream.eof()) {
                stream >> value;
                if (!value.empty())
                    m_visible_states.insert(value);
            }
            setVisible(m_visible_states.count(m_current_state)!=0);
        }
        void StateVisibleWidget::setVisibleStates(const sb::set<sb::string>& states) {
            m_visible_states = states;
            setVisible(m_visible_states.count(m_current_state)!=0);
        }
        
        void StateVisibleWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "VisibleStates") {
                setVisibleStates(_value);
            }
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
        
        bool StateVisibleWidget::_setWidgetState(const std::string& _value) {
            m_current_state = _value;
            setVisible(m_visible_states.count(_value)!=0);
            return Base::_setWidgetState(_value);
        }

    }
}
