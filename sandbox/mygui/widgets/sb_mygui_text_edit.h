
#ifndef __Sandbox__sb_mygui_text_edit__
#define __Sandbox__sb_mygui_text_edit__

#include "meta/sb_meta.h"
#include "MyGUI_EditBox.h"

namespace GHL {
    struct Event;
}

namespace Sandbox {
    
    namespace mygui {
        
        class TextEdit : public MyGUI::EditBox {
            MYGUI_RTTI_DERIVED( TextEdit )
        public:
            TextEdit();
            ~TextEdit();
            
            void setPropertyOverride(const std::string& _key, const std::string& _value);
            void initialiseOverride();
            void shutdownOverride();
            
            const std::string& getPlaceholder() const { return m_placeholder; }
            void setPlaceholder(const std::string& ph );
            
            virtual void setCaption(const MyGUI::UString& v);
        protected:
            std::string  m_placeholder;
            MyGUI::TextBox* m_placeholder_widget;
            void updatePlaceholder();
            virtual void updateEditState();
            virtual void updateCursorPosition();
            void onEvent(GHL::Event* e);
        };
    }
}

#endif /* __Sandbox__sb_mygui_text_edit__ */



