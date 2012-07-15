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
        
                
        static int lua_class_constructor_func( lua_State* L ) {
            int args_cnt = lua_gettop(L);
            
            lua_newtable(L);
            
            lua_pushliteral(L, "__init");
            lua_rawget(L, 1);
            if ( lua_isfunction(L, -1) ) {
                lua_pushvalue(L, -2);
                for (int i=2;i<=args_cnt;i++) {
                    lua_pushvalue(L, i);
                }
                lua_call_method(L, args_cnt, 0, "__init");
            } else {
                lua_pop(L, 1);
            }
            
            lua_pushliteral(L, "__metatable");
            lua_rawget(L, 1);
            
            
            lua_setmetatable(L, -2);
            return 1;
        }
        
        static int getter_indexer (lua_State *L)
		{
            sb_assert(lua_istable(L, 1));
            /// debug
          	lua_getmetatable(L, 1);             /// mt
          
            lua_pushliteral(L, "__class");
            lua_rawget(L, -2);                  /// mt class
            sb_assert(lua_istable(L, -1));
            
            lua_remove(L, -2);                  /// class
            
            do {
                
                // Look for the key in the metatable
				lua_pushvalue(L, 2);
				lua_rawget(L, -2);              /// class res
				
				// Did we get a non-nil result?  If so, return it
				if (!lua_isnil(L, -1)) {
                    lua_remove(L, -2);          /// res
                    return 1;
                }
				lua_pop(L, 1);                  ///  class
                
				lua_pushliteral(L, "__parent");
                lua_rawget(L, -2 );             /// class prnt
				if (lua_isnil(L, -1)) {
					lua_pop(L, 2);
                    lua_pushnil(L);
                    return 1;
                }
             	lua_remove(L, -2);              /// prnt
                lua_pushliteral(L, "__isnative");
                lua_rawget(L, -2);
                if (lua_toboolean(L, -1)) {
                    lua_pop(L, 2);
                    lua_pushliteral(L, "__native");
                    lua_rawget(L, 1);
                    if ( lua_isnil(L, -1)) {
                        lua_pop(L, 1);
                        lua_pushstring(L, "error getting native ptr");
                        lua_error(L);
                        return 0;
                    }
                    lua_pushvalue(L, 2);
                    lua_gettable(L,-2);
                    lua_remove(L, -2);
                    return 1;
                } 
                lua_pop(L, 1);
			} while (true);
			
			// Control never gets here
			return 0;
		}
        
        static int setter_indexer (lua_State *L)
		{
            sb_assert(lua_istable(L, 1));
            lua_pushliteral(L, "__native");
            lua_rawget(L, 1);
            if (lua_isuserdata(L, -1)) {
                lua_getmetatable(L, -1);             /// mt
                lua_remove(L, -2);
                do {
                    lua_pushliteral(L, "__props");
                    lua_rawget(L, -2);                  /// mt props
                    sb_assert(lua_istable(L, -1));
                    
                    // Look for the key in the metatable
                    lua_pushvalue(L, 2);
                    lua_rawget(L, -2);              /// mt props res
                    // Did we get a non-nil result?  If so, return it
                    if (!lua_isnil(L, -1)) {
                        lua_remove(L, -2);          /// mt res
                        lua_remove(L, -2);          /// res
                        lua_getfield(L, -1, "set"); /// res set
                        sb_assert(lua_isfunction(L, -1));
                        lua_remove(L, -2);          /// set
                        lua_pushvalue(L, 1);        /// set obj
                        lua_pushvalue(L, 3);        /// set obj val
                        lua_call_method(L, 2, 0,"set");
                        return 0;
                    }
                    lua_pop(L, 2);                  /// mt 
                    lua_pushliteral(L, "__parent");
                    lua_rawget(L, -2 );             /// mt prnt
                    if (lua_isnil(L, -1)) {
                        lua_pop(L, 2);
                        break;
                    }
                    lua_remove(L, -2);              /// prnt
                } while (true);
            } else {
                lua_pop(L, 1);
            }
            lua_pushvalue(L, 2);
            lua_pushvalue(L, 3);
            lua_rawset(L, 1);
            return 0;
      	}

        
        static int lua_class_superclass_func( lua_State* L ) {
            if (!lua_istable(L, 1)) {
                lua_argerror(L, 1, "super-class-table", 0);
                return 0;
            }
            lua_pushvalue(L,1);
            lua_setfield(L, lua_upvalueindex(1), "__parent");
            return 0;
        }
        
        static int lua_class_func( lua_State* L ) {
            if (!lua_isstring(L, 1)) {
                lua_argerror(L, 1, "class-name", 0);
                return 1;
            }
            const char* name = lua_tostring(L, 1);
            
            
            lua_newtable( L );
            
            
            lua_newtable( L );
            lua_pushcclosure(L, &lua_class_constructor_func, 0);
            lua_setfield(L, -2, "__call");
            lua_setmetatable(L, -2);
            
            lua_newtable( L );
            lua_pushcfunction(L, &getter_indexer);               /// mntbl raw_ptr indexer
			lua_setfield(L, -2, "__index");               /// mntbl raw_ptr
            lua_pushcfunction(L, &setter_indexer);               /// mntbl raw_ptr indexer
			lua_setfield(L, -2, "__newindex");               /// mntbl raw_ptr
            
            lua_pushvalue(L, -2);
            lua_setfield(L, -2, "__class");
            lua_setfield(L, -2, "__metatable");
            
            lua_pushvalue(L, -1);
            lua_setglobal(L, name);
            lua_pushcclosure(L, &lua_class_superclass_func, 1);
            return 1;
        }
        
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
