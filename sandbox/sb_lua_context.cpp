//
//  sb_lua_context.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/13/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_lua_context.h"


namespace Sandbox {
    
    LuaContext::LuaContext(  )  {
        
    }
    
    LuaContextPtr LuaContext::CreateInherited() {
        if (!Valid()) return LuaContextPtr();
        luabind::LuaVMHelperPtr helper = GetHelper();
        if (!helper) return LuaContextPtr();
        lua_State* L = helper->lua;
        LuaContextPtr ctx = sb::make_shared<LuaContext>();
        lua_newtable(L);    /// new ctx
        lua_newtable(L);
        GetObject(L);
        lua_setfield(L, -2, "__index"); // t mt
        lua_setmetatable(L, -2);
        ctx->SetObject(L);
        return ctx;
    }
    
    lua_State* LuaContext::get_state_with_table_on_top(const char* path) {
        if (!Valid()) return 0;
        luabind::LuaVMHelperPtr helper = GetHelper();
        if (!helper) return 0;
        lua_State* L = helper->lua;
        GetObject(L);
        luabind::lua_get_create_child_table(L,path);
        lua_pushstring(L, path);
        return L;
    }
    
    void LuaContext::set_value_on_top_of_stack_to_table(lua_State* L) {
        lua_rawset(L, -3);
    }
}
