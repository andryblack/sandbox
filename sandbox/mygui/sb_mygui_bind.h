//
//  sb_mygui_bind.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 24/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_mygui_bind__
#define __sr_osx__sb_mygui_bind__



struct lua_State;

namespace Sandbox {
    namespace mygui {
        void register_mygui( lua_State* lua );
    }
}

#endif /* defined(__sr_osx__sb_mygui_bind__) */
