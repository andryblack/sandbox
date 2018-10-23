#include "sb_mygui_text_edit.h"
#include "MyGUI_LanguageManager.h"
#include "MyGUI_InputManager.h"
#include "../sb_mygui_gui.h"
#include <ghl_event.h>


SB_META_DECLARE_OBJECT(Sandbox::mygui::TextEdit, MyGUI::EditBox)


namespace Sandbox {
    
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(TextEdit)
        
        TextEdit::TextEdit() : m_placeholder_widget(0) {
            
        }
        
        TextEdit::~TextEdit() {
            
        }
        
        void TextEdit::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "Placeholder") {
                setPlaceholder(MyGUI::LanguageManager::getInstance().replaceTags(_value));
            }
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
                               
       void TextEdit::initialiseOverride() {
           Base::initialiseOverride();
           
           ///@wskin_child{TextEdit, Widget, Placeholder}
           assignWidget(m_placeholder_widget, "Placeholder");
           if (m_placeholder_widget) {
               m_placeholder_widget->setCaption(m_placeholder);
           }
           GUI::getInstancePtr()->eventGHLEvent+=MyGUI::newDelegate( this, &TextEdit::onEvent );
       }
        
        void TextEdit::shutdownOverride() {
            GUI::getInstancePtr()->eventGHLEvent-=MyGUI::newDelegate( this, &TextEdit::onEvent );
            Base::shutdownOverride();
        }
        
        void TextEdit::setCaption(const MyGUI::UString& v) {
            Base::setCaption(v);
            updatePlaceholder();
        }
        
       void TextEdit::setPlaceholder(const std::string& value) {
           m_placeholder = value;
           if (m_placeholder_widget) {
               m_placeholder_widget->setCaption(m_placeholder);
           }
       }
        
        void TextEdit::updateEditState() {
            Base::updateEditState();
            updatePlaceholder();
        }
        
        void TextEdit::updateCursorPosition() {
            Base::updateCursorPosition();
            if (mIsFocus) {
                GUI::getInstancePtr()->showKeyboard(this);
            }
        }
        void TextEdit::updatePlaceholder() {
            if (m_placeholder_widget) {
                m_placeholder_widget->setVisible(
                                                 getCaption().empty() && !mIsPressed);
            }
        }
        
        void TextEdit::onEvent(GHL::Event* e) {
            if (MyGUI::InputManager::getInstance().getKeyFocusWidget()!=this)
                return;
            if (e->type == GHL::EVENT_TYPE_TEXT_INPUT_TEXT_CHANGED) {
                size_t len = strlen(e->data.text_input_text_changed.text);
                MyGUI::UString text = MyGUI::TextIterator::getOnlyText(e->data.text_input_text_changed.text);
                MyGUI::TextIterator::normaliseNewLine(text,mModeMultiline || mModeWordWrap);
                
                if (mOverflowToTheLeft)
                {
                    MyGUI::TextIterator::cutMaxLengthFromBeginning(text,mMaxTextLength);
                }
                else
                {
                    MyGUI::TextIterator::cutMaxLength(text,mMaxTextLength);
                }
                
                mTextLength = text.length();
                mCursorPosition = e->data.text_input_text_changed.cursor_position;
                if (mCursorPosition > mTextLength) {
                    mCursorPosition = mTextLength;
                }
                
                setRealString(text);
                
                if (len != strlen(text.c_str())) {
                    updateCursorPosition();
                } else {
                    Base::updateCursorPosition();
                }
                eventEditTextChange(this);
            } 
        }
    }
    
}
