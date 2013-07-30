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
        
        
        static const char* helper_name = "__lua_vm_helper";

        
        LuaVMHelperPtr GetHelper( lua_State* L ) {
            LUA_CHECK_STACK(0)
            LuaVMHelperPtr res;
            lua_getglobal(L, helper_name);
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
            lua_setglobal(L, helper_name);
            
            lua_pushcfunction(L, &lua_class_func);
            lua_setglobal(L, "class");
            
            LUA_CHECK_STACK(0)
            
            lua_create_metatable(L);
            lua_register_metatable(L,meta::type<meta::object>::info());
        }
        
        void Deinitialize( lua_State* L ) {
            lua_getglobal(L, helper_name);
            if ( lua_isuserdata(L, -1) ) {
                LuaVMHelperPtr* helper = reinterpret_cast<LuaVMHelperPtr*>(lua_touserdata(L, -1));
                helper->~LuaVMHelperPtr();
            }
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_setglobal(L, helper_name);
        }
    }
}
