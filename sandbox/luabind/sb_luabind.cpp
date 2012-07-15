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
        
                

        
        LuaVMHelperPtr GetHelper( lua_State* L ) {
            LuaVMHelperPtr res;
            lua_getglobal(L, "__lua_vm_helper");
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
            lua_setglobal(L, "__lua_vm_helper");
            
            lua_pushcfunction(L, &lua_class_func);
            lua_setglobal(L, "class");
            
            lua_stack_check sc(L);
            lua_create_metatable(L);
            lua_register_metatable(L,meta::type<meta::object>::info());
        }
        
        void Deinitialize( lua_State* L ) {
            lua_getglobal(L, "__lua_vm_helper");
            if ( lua_isuserdata(L, -1) ) {
                LuaVMHelperPtr* helper = reinterpret_cast<LuaVMHelperPtr*>(lua_touserdata(L, -1));
                helper->~LuaVMHelperPtr();
            }
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_setglobal(L, "__lua_vm_helper");
        }
    }
}
