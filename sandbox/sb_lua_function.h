//
//  sb_lua_function.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/14/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_lua_function__
#define __sr_osx__sb_lua_function__

#include "luabind/sb_luabind_ref.h"
#include <sbstd/sb_shared_ptr.h>

namespace Sandbox {
    
    class LuaContext;
    typedef sb::shared_ptr<LuaContext> LuaContextPtr;
    
    class LuaFunction : public luabind::LuaReference {
    public:
        explicit LuaFunction();
        bool    Execute();
        
        bool    SetSource( const LuaContextPtr& ctx, const char* src);
    };
    typedef sb::shared_ptr<LuaFunction> LuaFunctionPtr;
}

#endif /* defined(__sr_osx__sb_lua_function__) */
