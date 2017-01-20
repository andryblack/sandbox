//
//  sb_lua_context.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/13/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_lua_context.h"
#include "sb_lua_value.h"

namespace Sandbox {
    
    

    
    LuaContext::LuaContext(  )  {
        
    }
    
    LuaContextPtr LuaContext::CreateInherited() {
        if (!Valid()) return LuaContextPtr();
        luabind::LuaVMHelperPtr helper = GetHelper();
        if (!helper) return LuaContextPtr();
        lua_State* L = helper->lua;
        LuaContextPtr ctx(new LuaContext);
        lua_newtable(L);    /// new ctx
        lua_newtable(L);
        GetObject(L);
        lua_setfield(L, -2, "__index"); // t mt
        lua_setmetatable(L, -2);
        ctx->SetObject(L);
        return ctx;
    }
    
    sb::intrusive_ptr<LuaContext> LuaContext::Create() {
        if (!Valid()) return LuaContextPtr();
        luabind::LuaVMHelperPtr helper = GetHelper();
        if (!helper) return LuaContextPtr();
        lua_State* L = helper->lua;
        LUA_CHECK_STACK(0)
        LuaContextPtr ctx(new LuaContext);
        GetObject(L);
        lua_newtable(L);
        ctx->SetObject(L);
        lua_pop(L, 1);
        return ctx;
    }
    
    sb::intrusive_ptr<LuaContext> LuaContext::CreateChild(const char* name) {
        if (!Valid()) return LuaContextPtr();
        luabind::LuaVMHelperPtr helper = GetHelper();
        if (!helper) return LuaContextPtr();
        lua_State* L = helper->lua;
        LUA_CHECK_STACK(0)
        LuaContextPtr ctx(new LuaContext);
        GetObject(L);
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, name);
        ctx->SetObject(L);
        lua_pop(L, 1);
        return ctx;
    }
    
    bool LuaContext::IsSame(const sb::intrusive_ptr<LuaContext>& o) {
        if (!o || !o->Valid()) return !Valid();
        luabind::LuaVMHelperPtr helper = GetHelper();
        if (!helper) return LuaContextPtr();
        lua_State* L = helper->lua;
        LUA_CHECK_STACK(0)
        GetObject(L);
        o->GetObject(L);
        int r = lua_rawequal(L, -1, -2);
        lua_pop(L, 2);
        return r!=0;
    }
    
    lua_State* LuaContext::get_state_with_table_on_top(const char* path) {
        if (!Valid()) return 0;
        luabind::LuaVMHelperPtr helper = GetHelper();
        if (!helper) return 0;
        lua_State* L = helper->lua;
        LUA_CHECK_STACK(2)
        GetObject(L);
        luabind::lua_get_create_child_table(L,path);
        lua_pushstring(L, path);
        return L;
    }
    
    lua_State* LuaContext::get_state_with_array_on_top() {
        if (!Valid()) return 0;
        luabind::LuaVMHelperPtr helper = GetHelper();
        if (!helper) return 0;
        lua_State* L = helper->lua;
        LUA_CHECK_STACK(2)
        GetObject(L);
        lua_len(L,-1);
        int len = lua_tointeger(L, -1);
        lua_pop(L, 1);
        lua_pushinteger(L, len + 1);
        return L;
    }
    
    void LuaContext::set_value_on_top_of_stack_to_table(lua_State* L) {
        LUA_CHECK_STACK(-3)
        lua_rawset(L, -3);
        lua_pop(L, 1);
    }
    
    
    lua_State* LuaValue::get_state_with_value_on_top() const {
        if (!Valid()) return 0;
        luabind::LuaVMHelperPtr helper = GetHelper();
        if (!helper) return 0;
        lua_State* L = helper->lua;
        LUA_CHECK_STACK(1)
        GetObject(L);
        return L;
    }
}
