#include "sb_mygui_gui.h"
#include "MyGUI_InputManager.h"
#include "MyGUI_ClipboardManager.h"
#include "MyGUI_LanguageManager.h"
#include "MyGUI_TextIterator.h"
#include "MyGUI_EditBox.h"

#include "widgets/sb_mygui_text_input.h"

#include "sb_lua_context.h"
#include <ghl_system.h>

namespace Sandbox {
    namespace mygui  {
        
        GUI::GUI( GHL::System* s) : m_system(s) {
            
        }
        
        void GUI::initialize(const LuaContextPtr& ctx) {
            m_ctx = ctx;
            
            MyGUI::Gui::initialise();
            
            mLanguageManager->eventRequestTag =
                MyGUI::newDelegate(this,&GUI::get_mygui_localization);
            mInputManager->eventChangeKeyFocus +=
                MyGUI::newDelegate(this,&GUI::mygui_change_key_focus);
            mClipboardManager->eventClipboardChanged +=
                MyGUI::newDelegate(this,&GUI::mygui_clipboard_changed);
            mClipboardManager->eventClipboardRequested +=
                MyGUI::newDelegate(this,&GUI::mygui_clipboard_requested);
            
        }
        
        
        void GUI::get_mygui_localization(const MyGUI::UString& key, MyGUI::UString& value) {
            LuaContextPtr localization = m_ctx->GetValue<LuaContextPtr>("TRSTR");
            if (localization) {
                value.assign( localization->GetValueRaw<sb::string>(key.asUTF8_c_str()) );
            }
        }
        void GUI::mygui_change_key_focus( MyGUI::Widget* w ) {
            if (m_ctx) {
                if (m_ctx->GetValue<bool>("application.onKeyboardFocusChanged")) {
                    m_ctx->GetValue<LuaContextPtr>("application")
                        ->call("onKeyboardFocusChanged",static_cast<MyGUI::Widget*>(0));
                }
            }
            if (w) {
                showKeyboard(w);
            } else {
                m_system->HideKeyboard();
            }
        }
        void GUI::mygui_clipboard_changed( const std::string& type, const std::string& text ) {
            m_clipboard_text = MyGUI::TextIterator::getOnlyText(MyGUI::UString(text)).asUTF8_c_str();
        }
        void GUI::mygui_clipboard_requested( const std::string& type, std::string& text  ) {
            text.assign(m_clipboard_text);
        }
        
        void GUI::showKeyboard(MyGUI::Widget *widget) {
            GHL::TextInputConfig config;
            config.system_input = true;
            config.placeholder = 0;
            config.accept_button = GHL::TIAB_DONE;
            if (widget->getUserString("accept_button")=="send") {
                config.accept_button = GHL::TIAB_SEND;
            }
            if (widget->isType(MYGUI_RTTI_GET_TYPE(MyGUI::EditBox))) {
                config.system_input = false;
                m_system->ShowKeyboard(&config);
            } else if (widget->isType(MYGUI_RTTI_GET_TYPE(TextInput))) {
                config.system_input = true;
                config.placeholder = widget->castType<TextInput>()->getPlaceholder().c_str();
                m_system->ShowKeyboard(&config);
            } else {
                m_system->HideKeyboard();
            }
            
        }

    }
}
