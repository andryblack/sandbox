//
//  sb_luabind.cpp
//  YinYang
//
//  Created by Андрей Куницын on 7/14/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "sb_luabind.h"

namespace Sandbox {
    
    namespace luabind {
        
        
        static const char* helper_idx = "__lua_vm_helper";
        static const char* error_handler_idx = "__lua_vm_error_handler";

        void PushErrorHandler(lua_State* L) {
            lua_rawgetp(L, LUA_REGISTRYINDEX, error_handler_idx);
        }
        void SetErrorHandler(lua_State* L, lua_CFunction func) {
            lua_pushcclosure(L, func, 0);
            lua_rawsetp(L, LUA_REGISTRYINDEX, error_handler_idx);
        }
        LuaVMHelperPtr GetHelper( lua_State* L ) {
            if (!L) {
                return LuaVMHelperPtr();
            }
            LUA_CHECK_STACK(0)
            LuaVMHelperPtr res;
            lua_rawgetp(L, LUA_REGISTRYINDEX, helper_idx);
            if ( lua_isuserdata(L, -1) ) {
                LuaVMHelperPtr* helper = reinterpret_cast<LuaVMHelperPtr*>(lua_touserdata(L, -1));
                res = *helper;
            }
            lua_pop(L, 1);
            return res;
        }
        
        void Initialize( lua_State* L ) {
            
            void* data = lua_newuserdata(L, sizeof(LuaVMHelperPtr));
            LuaVMHelperPtr* helper = new (data) LuaVMHelperPtr(new LuaVMHelper());
            (*helper)->lua = L;
            
            lua_rawsetp(L, LUA_REGISTRYINDEX, helper_idx);
            
            LUA_CHECK_STACK(0)
            
            lua_create_metatable(L);
            lua_register_metatable(L,meta::type<meta::object>::info());
        }
        
        void Deinitialize( lua_State* L ) {
            lua_rawgetp(L, LUA_REGISTRYINDEX, helper_idx);
            if ( lua_isuserdata(L, -1) ) {
                LuaVMHelperPtr* helper = reinterpret_cast<LuaVMHelperPtr*>(lua_touserdata(L, -1));
                helper->~LuaVMHelperPtr();
            }
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_rawsetp(L, LUA_REGISTRYINDEX, helper_idx);
        }
        
        Namespace::Namespace(lua_State* L, const char* name) : impl::namespace_registrator(L),m_name(name) {
            lua_create_metatable(L);
        }
        
        Namespace::~Namespace() {
            luabind::lua_set_value(m_L,m_name);
        }
    }
}
