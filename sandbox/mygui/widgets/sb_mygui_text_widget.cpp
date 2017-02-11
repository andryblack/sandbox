#include "sb_mygui_text_widget.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::TextWidget, MyGUI::TextBox)


namespace Sandbox {
    
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(TextWidget)

        TextWidget::TextWidget() {
            
        }
        
        TextWidget::~TextWidget() {
            
        }
        
        bool TextWidget::getWordWrap() const {
            if (nullptr != getSubWidgetText())
                return getSubWidgetText()->getWordWrap();
            return false;
        }
        
        void TextWidget::setWordWrap(bool wrap) {
            if (nullptr != getSubWidgetText())
                getSubWidgetText()->setWordWrap(wrap);
        }

        
        void TextWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "WordWrap") {
                setWordWrap(MyGUI::utility::parseValue<bool>(_value));
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
