#include "sb_lua_value.h"

namespace Sandbox {
    
    lua_State* LuaValue::get_state_with_value_on_top() const {
        if (!Valid()) return 0;
        luabind::LuaVMHelperPtr helper = GetHelper();
        if (!helper) return 0;
        lua_State* L = helper->lua;
        LUA_CHECK_STACK(1)
        GetObject(L);
        return L;
    }
    
    int LuaValue::GetLuaType() const {
        lua_State* L = get_state_with_value_on_top();
        if (!L) return LUA_TNONE;
        LUA_CHECK_STACK(-1)
        int res = lua_type(L,-1);
        lua_pop(L,1);
        return res;
    }
    
    void LuaValue::Traverse(LuaTableTraverser& tr) const {
        lua_State* L = get_state_with_value_on_top();
        if (!L) return;
        LUA_CHECK_STACK(-1)
        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            return;
        }
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            /* uses 'key' (at index -2) and 'value' (at index -1) */
            LUA_CHECK_STACK(-1)
            LuaValue key;
            lua_pushvalue(L, -2);
            key.SetObject(L);
            LuaValue value;
            value.SetObject(L);
            tr.OnKeyValue(key, value);
        }
        lua_pop(L,1);
    }
}
