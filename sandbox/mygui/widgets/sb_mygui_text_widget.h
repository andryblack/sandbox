//
//  sb_mygui_scene_widget.h
//  Sandbox
//
//
//

#ifndef __Sandbox__sb_mygui_text_widget__
#define __Sandbox__sb_mygui_text_widget__

#include "meta/sb_meta.h"
#include "MyGUI_TextBox.h"


namespace Sandbox {
    
    namespace mygui {
        
        class TextWidget : public MyGUI::TextBox {
            MYGUI_RTTI_DERIVED( TextWidget )
        public:
            TextWidget();
            ~TextWidget();
            
            void setPropertyOverride(const std::string& _key, const std::string& _value);
        protected:
        };
    }
}

#endif /* defined(__Sandbox__sb_mygui_text_widget__) */



