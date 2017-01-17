#ifndef _SB_MYGUI_TEXT_INPUT_H_INCLUDED_
#define _SB_MYGUI_TEXT_INPUT_H_INCLUDED_

#include "sb_mygui_text_widget.h"
#include "MyGUI_EditBox.h"

namespace GHL {
    struct Event;
}

namespace Sandbox {
    namespace mygui {
        
        class TextInput;
        
        typedef MyGUI::delegates::CMultiDelegate1<TextInput*> EventHandle_TextInputPtr;
        
        class TextInput : public TextWidget {
            MYGUI_RTTI_DERIVED( TextInput )
        private:
            class InputHandler;
            sb::string m_placeholder;
            MyGUI::TextBox*      m_client;
            MyGUI::TextBox*      m_placeholder_widget;
        public:
            
            void emitAccept();
            
            /*events:*/
            /** Event : Enter pressed (Ctrl+enter in multiline mode).\n
             signature : void method(MyGUI::EditBox* _sender)
             @param _sender widget that called this event
             */
            EventHandle_TextInputPtr eventEditAccept;
            
            /** Event : Text changed.\n
             signature : void method(MyGUI::EditBox* _sender)
             @param _sender widget that called this event
             */
            EventHandle_TextInputPtr eventEditTextChange;
            
            void setPlaceholder(const sb::string& text);
            const sb::string& getPlaceholder() const { return m_placeholder; }
            
            /** Set widget caption */
            virtual void setCaption(const MyGUI::UString& _value);
            /** Get widget caption */
            virtual const MyGUI::UString& getCaption() const;
            
            
        public:
            TextInput();
            ~TextInput();
        protected:
            virtual void shutdownOverride();
            virtual void initialiseOverride();
            void setPropertyOverride(const std::string& _key, const std::string& _value);
            void onEvent(GHL::Event* e);
          
            
        };
    }
}


#endif /*_SB_MYGUI_TEXT_INPUT_H_INCLUDED_*/
