#include "sb_mygui_text_widget.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::TextWidget, MyGUI::TextBox)


namespace Sandbox {
    
    namespace mygui {

        TextWidget::TextWidget() {
            
        }
        
        TextWidget::~TextWidget() {
            
        }

        
        void TextWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "WorldWrap") {
                if (nullptr != getSubWidgetText())
                    getSubWidgetText()->setWordWrap(MyGUI::utility::parseValue<bool>(_value));
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
