#include "luabind/sb_luabind.h"
#include "meta/sb_meta.h"
#include <ghl_keys.h>

SB_META_DECLARE_KLASS(GHL::MouseButton,void);
SB_META_ENUM_BIND(GHL::MouseButton,namespace GHL,
                  SB_META_ENUM_ITEM(MOUSE_BUTTON_NONE)
                  SB_META_ENUM_ITEM(MOUSE_BUTTON_LEFT)
                  SB_META_ENUM_ITEM(MOUSE_BUTTON_RIGHT)
                  SB_META_ENUM_ITEM(MOUSE_BUTTON_MIDDLE)
                  )

SB_META_DECLARE_KLASS(GHL::KeyMod,void);
SB_META_ENUM_BIND(GHL::KeyMod,namespace GHL,
                  SB_META_ENUM_ITEM(KEYMOD_SHIFT)
                  SB_META_ENUM_ITEM(KEYMOD_ALT)
                  SB_META_ENUM_ITEM(KEYMOD_CTRL)
                  SB_META_ENUM_ITEM(KEYMOD_COMMAND)
                  )

SB_META_DECLARE_KLASS(GHL::Key,void);
SB_META_ENUM_BIND(GHL::Key,namespace GHL,
                  SB_META_ENUM_ITEM(KEY_NONE)
                  
                  SB_META_ENUM_ITEM(KEY_ESCAPE)
                  SB_META_ENUM_ITEM(KEY_BACKSPACE)
                  SB_META_ENUM_ITEM(KEY_TAB)
                  SB_META_ENUM_ITEM(KEY_ENTER)
                  SB_META_ENUM_ITEM(KEY_SPACE)
                  
                  SB_META_ENUM_ITEM(KEY_SHIFT)
                  SB_META_ENUM_ITEM(KEY_CTRL)
                  SB_META_ENUM_ITEM(KEY_ALT)
                  
                  SB_META_ENUM_ITEM(KEY_LWIN)
                  SB_META_ENUM_ITEM(KEY_RWIN)
                  SB_META_ENUM_ITEM(KEY_APPS)
                  
                  SB_META_ENUM_ITEM(KEY_PAUSE)
                  SB_META_ENUM_ITEM(KEY_CAPSLOCK)
                  SB_META_ENUM_ITEM(KEY_NUMLOCK)
                  SB_META_ENUM_ITEM(KEY_SCROLLLOCK)
                  
                  SB_META_ENUM_ITEM(KEY_PGUP)
                  SB_META_ENUM_ITEM(KEY_PGDN)
                  SB_META_ENUM_ITEM(KEY_HOME)
                  SB_META_ENUM_ITEM(KEY_END)
                  SB_META_ENUM_ITEM(KEY_INSERT)
                  SB_META_ENUM_ITEM(KEY_DELETE)
                  
                  SB_META_ENUM_ITEM(KEY_LEFT)
                  SB_META_ENUM_ITEM(KEY_UP)
                  SB_META_ENUM_ITEM(KEY_RIGHT)
                  SB_META_ENUM_ITEM(KEY_DOWN)
                  
                  SB_META_ENUM_ITEM(KEY_0)
                  SB_META_ENUM_ITEM(KEY_1)
                  SB_META_ENUM_ITEM(KEY_2)
                  SB_META_ENUM_ITEM(KEY_3)
                  SB_META_ENUM_ITEM(KEY_4)
                  SB_META_ENUM_ITEM(KEY_5)
                  SB_META_ENUM_ITEM(KEY_6)
                  SB_META_ENUM_ITEM(KEY_7)
                  SB_META_ENUM_ITEM(KEY_8)
                  SB_META_ENUM_ITEM(KEY_9)
                  
                  SB_META_ENUM_ITEM(KEY_A)
                  SB_META_ENUM_ITEM(KEY_B)
                  SB_META_ENUM_ITEM(KEY_C)
                  SB_META_ENUM_ITEM(KEY_D)
                  SB_META_ENUM_ITEM(KEY_E)
                  SB_META_ENUM_ITEM(KEY_F)
                  SB_META_ENUM_ITEM(KEY_G)
                  SB_META_ENUM_ITEM(KEY_H)
                  SB_META_ENUM_ITEM(KEY_I)
                  SB_META_ENUM_ITEM(KEY_J)
                  SB_META_ENUM_ITEM(KEY_K)
                  SB_META_ENUM_ITEM(KEY_L)
                  SB_META_ENUM_ITEM(KEY_M)
                  SB_META_ENUM_ITEM(KEY_N)
                  SB_META_ENUM_ITEM(KEY_O)
                  SB_META_ENUM_ITEM(KEY_P)
                  SB_META_ENUM_ITEM(KEY_Q)
                  SB_META_ENUM_ITEM(KEY_R)
                  SB_META_ENUM_ITEM(KEY_S)
                  SB_META_ENUM_ITEM(KEY_T)
                  SB_META_ENUM_ITEM(KEY_U)
                  SB_META_ENUM_ITEM(KEY_V)
                  SB_META_ENUM_ITEM(KEY_W)
                  SB_META_ENUM_ITEM(KEY_X)
                  SB_META_ENUM_ITEM(KEY_Y)
                  SB_META_ENUM_ITEM(KEY_Z)
                  
                  SB_META_ENUM_ITEM(KEY_GRAVE)
                  SB_META_ENUM_ITEM(KEY_MINUS)
                  SB_META_ENUM_ITEM(KEY_EQUALS)
                  SB_META_ENUM_ITEM(KEY_BACKSLASH)
                  SB_META_ENUM_ITEM(KEY_LBRACKET)
                  SB_META_ENUM_ITEM(KEY_RBRACKET)
                  SB_META_ENUM_ITEM(KEY_SEMICOLON)
                  SB_META_ENUM_ITEM(KEY_APOSTROPHE)
                  SB_META_ENUM_ITEM(KEY_COMMA)
                  SB_META_ENUM_ITEM(KEY_PERIOD)
                  SB_META_ENUM_ITEM(KEY_SLASH)
                  
                  SB_META_ENUM_ITEM(KEY_NUMPAD0)
                  SB_META_ENUM_ITEM(KEY_NUMPAD1)
                  SB_META_ENUM_ITEM(KEY_NUMPAD2)
                  SB_META_ENUM_ITEM(KEY_NUMPAD3)
                  SB_META_ENUM_ITEM(KEY_NUMPAD4)
                  SB_META_ENUM_ITEM(KEY_NUMPAD5)
                  SB_META_ENUM_ITEM(KEY_NUMPAD6)
                  SB_META_ENUM_ITEM(KEY_NUMPAD7)
                  SB_META_ENUM_ITEM(KEY_NUMPAD8)
                  SB_META_ENUM_ITEM(KEY_NUMPAD9)
                  
                  SB_META_ENUM_ITEM(KEY_MULTIPLY)
                  SB_META_ENUM_ITEM(KEY_DIVIDE)
                  SB_META_ENUM_ITEM(KEY_ADD)
                  SB_META_ENUM_ITEM(KEY_SUBTRACT)
                  SB_META_ENUM_ITEM(KEY_DECIMAL)
                  
                  SB_META_ENUM_ITEM(KEY_F1)
                  SB_META_ENUM_ITEM(KEY_F2)
                  SB_META_ENUM_ITEM(KEY_F3)
                  SB_META_ENUM_ITEM(KEY_F4)
                  SB_META_ENUM_ITEM(KEY_F5)
                  SB_META_ENUM_ITEM(KEY_F6)
                  SB_META_ENUM_ITEM(KEY_F7)
                  SB_META_ENUM_ITEM(KEY_F8)
                  SB_META_ENUM_ITEM(KEY_F9)
                  SB_META_ENUM_ITEM(KEY_F10)
                  SB_META_ENUM_ITEM(KEY_F11)
                  SB_META_ENUM_ITEM(KEY_F12)

                  SB_META_ENUM_ITEM(KEY_BACK)
                  
                  )


namespace Sandbox {
    
    
    void register_keys( lua_State* lua ) {
        luabind::Enum<GHL::MouseButton>(lua);
        luabind::Enum<GHL::KeyMod>(lua);
        luabind::Enum<GHL::Key>(lua);
    }

}