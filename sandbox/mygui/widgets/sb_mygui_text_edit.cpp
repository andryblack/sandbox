#include "sb_mygui_text_edit.h"
#include "MyGUI_LanguageManager.h"

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
        void TextEdit::updatePlaceholder() {
            if (m_placeholder_widget) {
                m_placeholder_widget->setVisible(
                                                 getCaption().empty() && !mIsPressed);
            }
        }
    }
    
}
