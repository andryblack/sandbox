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
               
                case GHL::KEY_LEFT:         return MyGUI::KeyCode::ArrowLeft;
                case GHL::KEY_RIGHT:        return MyGUI::KeyCode::ArrowRight;
                case GHL::KEY_UP:           return MyGUI::KeyCode::ArrowUp;
                case GHL::KEY_DOWN:         return MyGUI::KeyCode::ArrowDown;
                    
                    
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
                
                case GHL::KEY_A: return MyGUI::KeyCode::A;
                case GHL::KEY_B: return MyGUI::KeyCode::B;
                case GHL::KEY_C: return MyGUI::KeyCode::C;
                case GHL::KEY_D: return MyGUI::KeyCode::D;
                case GHL::KEY_E: return MyGUI::KeyCode::E;
                case GHL::KEY_F: return MyGUI::KeyCode::F;
                case GHL::KEY_G: return MyGUI::KeyCode::G;
                case GHL::KEY_H: return MyGUI::KeyCode::H;
                case GHL::KEY_I: return MyGUI::KeyCode::I;
                case GHL::KEY_J: return MyGUI::KeyCode::J;
                case GHL::KEY_K: return MyGUI::KeyCode::K;
                case GHL::KEY_L: return MyGUI::KeyCode::L;
                case GHL::KEY_M: return MyGUI::KeyCode::M;
                case GHL::KEY_N: return MyGUI::KeyCode::N;
                case GHL::KEY_O: return MyGUI::KeyCode::O;
                case GHL::KEY_P: return MyGUI::KeyCode::P;
                case GHL::KEY_Q: return MyGUI::KeyCode::Q;
                case GHL::KEY_R: return MyGUI::KeyCode::R;
                case GHL::KEY_S: return MyGUI::KeyCode::S;
                case GHL::KEY_T: return MyGUI::KeyCode::T;
                case GHL::KEY_U: return MyGUI::KeyCode::U;
                case GHL::KEY_V: return MyGUI::KeyCode::V;
                case GHL::KEY_W: return MyGUI::KeyCode::W;
                case GHL::KEY_X: return MyGUI::KeyCode::X;
                case GHL::KEY_Y: return MyGUI::KeyCode::Y;
                case GHL::KEY_Z: return MyGUI::KeyCode::Z;
                    
                

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