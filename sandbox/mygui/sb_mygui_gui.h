#ifndef _SB_MYGUI_GUI_H_INCLUDED_
#define _SB_MYGUI_GUI_H_INCLUDED_

#include "MyGUI_Gui.h"

#include <ghl_system.h>
#include <sbstd/sb_intrusive_ptr.h>
#include <sbstd/sb_string.h>

namespace GHL {
    struct Event;
}
namespace Sandbox {
    
    class LuaContext;
    typedef sb::intrusive_ptr<LuaContext> LuaContextPtr;
    
    namespace mygui {
        
        typedef MyGUI::delegates::CMultiDelegate1<GHL::Event*> EventHandle_GHLEventPtr;
        
        class GUI : public MyGUI::Gui {
        public:
            explicit GUI(GHL::System* system);
            
            virtual void initialize(const LuaContextPtr& ctx);
            
            void showKeyboard(MyGUI::Widget* widget);
            
            static GUI* getInstancePtr() {
                return static_cast<GUI*>(MyGUI::Gui::getInstancePtr());
            }
            GHL::System* getSystem() { return m_system; }
            void setCursor(GHL::SystemCursor cursor);
            
            EventHandle_GHLEventPtr eventGHLEvent;
        protected:
            virtual void get_mygui_localization(const MyGUI::UString & key,MyGUI::UString& value);
            void mygui_change_key_focus( MyGUI::Widget* w );
            void mygui_change_mouse_focus( MyGUI::Widget* w);
            void mygui_clipboard_changed( const std::string& type, const std::string& text );
            void mygui_clipboard_requested( const std::string& type, std::string& text  );
        private:
            GHL::System*    m_system;
            LuaContextPtr   m_ctx;
            sb::string m_clipboard_text;
        };
        
        
    }
}

#endif /*_SB_MYGUI_GUI_H_INCLUDED_*/
