#ifndef _SB_LUA_VALUE_H_INCLUDED_
#define _SB_LUA_VALUE_H_INCLUDED_

#include "luabind/sb_luabind_ref.h"
#include "luabind/impl/sb_luabind_wrapper_base.h"
#include "luabind/sb_luabind_stack.h"
#include "sb_notcopyable.h"
#include <sbstd/sb_traits.h>
#include <sbstd/sb_intrusive_ptr.h>

namespace Sandbox {
    
    class LuaValue : public luabind::LuaReference {
    private:
        lua_State* get_state_with_value_on_top() const;
    public:
        explicit LuaValue() {}
        
        template <class T>
        inline T GetAs() const {
            lua_State* L = get_state_with_value_on_top();
            if (!L) return T();
            LUA_CHECK_STACK(-1)
            T val = luabind::stack<T>::get(L,-1);
            lua_pop(L,1);
            return val;
        }
        
        inline int GetLuaType() const {
            lua_State* L = get_state_with_value_on_top();
            if (!L) return LUA_TNONE;
            LUA_CHECK_STACK(-1)
            int res = lua_type(L,-1);
            lua_pop(L,1);
            return res;
        }
        
    };
    typedef sb::intrusive_ptr<LuaValue> LuaValuePtr;
    
    namespace luabind {
        template <>
        struct stack<LuaValuePtr> : stack_lua_object_ptr<LuaValue> {};
        template <>
        struct stack<const LuaValuePtr&> : stack_lua_object_ptr<LuaValue> {};
    }

    
}
#endif /*_SB_LUA_VALUE_H_INCLUDED_*/