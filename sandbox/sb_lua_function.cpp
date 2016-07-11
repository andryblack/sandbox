//
//  sb_lua_function.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/14/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_lua_function.h"
#include "sb_lua_context.h"
#include "sb_log.h"
#include "luabind/sb_luabind.h"

namespace Sandbox {
    
    static const char* MODULE = "LuaFunction";

    LuaFunction::LuaFunction() {
        
    }
    
    bool    LuaFunction::SetSource( const LuaContextPtr& ctx, const char* src) {
        sb_assert(ctx);
        luabind::LuaVMHelperPtr helper = ctx->GetHelper();
        sb_assert(helper);
        lua_State* L = helper->lua;
        int res = luaL_loadstring(L, src);
        if (res!=0) {
			LogError(MODULE) << "Failed to load script: " << src;
            LogError(MODULE) << lua_tostring(L, -1);
            lua_pop(L, 1);
			return false;
		}
        ctx->GetObject(L);
        lua_setupvalue(L, -2, 1);
        SetObject(L);
        //LogInfo(MODULE) << "pcall <<<< top : " << lua_gettop(m_L);
        return true;
    }
    
    
    
    bool LuaFunction::Execute() {
        if (!Valid()) return false;
        luabind::LuaVMHelperPtr helper = GetHelper();
        if (!helper) return false;
        lua_State* L = helper->lua;
        LUA_CHECK_STACK(0)
        luabind::PushErrorHandler(L);
        int traceback_index = lua_gettop(L);
        GetObject(L);
        int res = lua_pcall(L, 0, 0, -2);
        if (res) {
            LogError(MODULE) << "pcall : " << res;
            LogError(MODULE) << lua_tostring(L, -1);
            lua_pop(L, 2);
            return false;
        }
        lua_pop(L, lua_gettop(L)-traceback_index+1);
        return true;
    }
    
}
