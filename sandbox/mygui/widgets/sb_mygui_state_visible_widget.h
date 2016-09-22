#ifndef _SB_MYGUI_STATE_VISIBLE_WIDGET_H_INCLUDED_
#define _SB_MYGUI_STATE_VISIBLE_WIDGET_H_INCLUDED_

#include "meta/sb_meta.h"
#include "MyGUI_Widget.h"
#include <sbstd/sb_set.h>
#include <sbstd/sb_string.h>

namespace Sandbox {
    
    namespace mygui {
        
        class StateVisibleWidget : public MyGUI::Widget {
            MYGUI_RTTI_DERIVED( StateVisibleWidget )
        public:
            StateVisibleWidget();
            ~StateVisibleWidget();
            
            
            void setVisibleStates(const sb::string& states);
            void setVisibleStates(const sb::set<sb::string>& states);
            const sb::set<sb::string>& getVisibleStates() const {
                return m_visible_states;
            }

            void setPropertyOverride(const std::string& _key, const std::string& _value);
        protected:
            virtual bool _setWidgetState(const std::string& _value);
            sb::set<sb::string> m_visible_states;
            sb::string m_current_state;
        };
    }
}

#endif /* _SB_MYGUI_SELECTABLE_WIDGET_H_INCLUDED_ */

