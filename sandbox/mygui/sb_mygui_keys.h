//
//  sb_mygui_keys.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 24/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_mygui_keys__
#define __sr_osx__sb_mygui_keys__

#include <ghl_keys.h>
#include "MyGUI_KeyCode.h"
#include "MyGUI_MouseButton.h"

namespace Sandbox {
    
    namespace mygui {
        
        MyGUI::KeyCode translate_key( GHL::Key k );
        MyGUI::MouseButton translate_key( GHL::MouseButton b );
        
    }
    
}

#endif /* defined(__sr_osx__sb_mygui_keys__) */
