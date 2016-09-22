#ifndef _SB_MYGUI_SELECTABLE_WIDGET_H_INCLUDED_
#define _SB_MYGUI_SELECTABLE_WIDGET_H_INCLUDED_

#include "meta/sb_meta.h"
#include "MyGUI_Widget.h"


namespace Sandbox {
    
    namespace mygui {
        
        class SelectableWidget : public MyGUI::Widget {
            MYGUI_RTTI_DERIVED( SelectableWidget )
        public:
            SelectableWidget();
            ~SelectableWidget();
            
            void setSelected(bool selected);
            bool getSelected() const { return m_selected; }
            
            void setPropertyOverride(const std::string& _key, const std::string& _value);
        protected:
            bool m_selected;
            virtual void baseUpdateEnable();
            void updateState();
        };
    }
}

#endif /* _SB_MYGUI_SELECTABLE_WIDGET_H_INCLUDED_ */



