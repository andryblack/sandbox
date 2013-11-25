//
//  sb_mygui_keys.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 24/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_mygui_keys.h"

namespace Sandbox {
    
    namespace mygui {
    
        MyGUI::KeyCode translate_key( GHL::Key k ) {
            switch (k) {
                    
                case GHL::KEY_ESCAPE:       return MyGUI::KeyCode::Escape;
                case GHL::KEY_BACKSPACE:    return MyGUI::KeyCode::Backspace;
                case GHL::KEY_TAB:          return MyGUI::KeyCode::Tab;
                case GHL::KEY_ENTER:        return MyGUI::KeyCode::Return;
                case GHL::KEY_SPACE:        return MyGUI::KeyCode::Space;
                    
                case GHL::KEY_0: return MyGUI::KeyCode::Zero;
                case GHL::KEY_1: return MyGUI::KeyCode::One;
                case GHL::KEY_2: return MyGUI::KeyCode::Two;
                case GHL::KEY_3: return MyGUI::KeyCode::Three;
                case GHL::KEY_4: return MyGUI::KeyCode::Four;
                case GHL::KEY_5: return MyGUI::KeyCode::Five;
                case GHL::KEY_6: return MyGUI::KeyCode::Six;
                case GHL::KEY_7: return MyGUI::KeyCode::Seven;
                case GHL::KEY_8: return MyGUI::KeyCode::Eight;
                case GHL::KEY_9: return MyGUI::KeyCode::Nine;
                    
                

                default:
                    break;
            }
            return MyGUI::KeyCode::None;
        }
        
        MyGUI::MouseButton translate_key( GHL::MouseButton b ) {
            switch (b) {
                case GHL::MOUSE_BUTTON_LEFT:    return MyGUI::MouseButton::Left;
                case GHL::MOUSE_BUTTON_RIGHT:   return MyGUI::MouseButton::Right;
                default:
                    break;
            }
            return MyGUI::MouseButton::None;
        }
    }
    
}