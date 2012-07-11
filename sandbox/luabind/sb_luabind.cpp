//
//  sb_luabind.cpp
//  YinYang
//
//  Created by Андрей Куницын on 6/9/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "sb_luabind.h"



#include "../sb_inplace_string.h"

namespace Sandbox {
    
    namespace luabind {
        
        
                

        
        
        
        static int destructor_func( lua_State* L ) {
            if ( lua_isuserdata(L, 1 ) ) {
                data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, 1));
                if ( holder->destructor ) {
                    holder->destructor( holder+1 );
                }
            }
            return 0;
        }
        
       
        
        static int getter_indexer (lua_State *L)
		{
            sb_assert(lua_isuserdata(L, 1));
/// debug
#if 0
            data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, 1));
            if (strcmp(holder->info->name,"Sandbox::Chipmunk::TransformAdapter")==0) {
                int a = 0;
            }
#endif
          	lua_getmetatable(L, 1);             /// mt
            do {
                lua_pushliteral(L, "props");
                lua_rawget(L, -2);                  /// mt props
                sb_assert(lua_istable(L, -1));
                
                // Look for the key in the metatable
				lua_pushvalue(L, 2);
				lua_rawget(L, -2);              /// mt props res
				// Did we get a non-nil result?  If so, return it
				if (!lua_isnil(L, -1)) {
                    lua_remove(L, -2);          /// mt res
                    lua_remove(L, -2);          /// res
                    lua_getfield(L, -1, "get"); /// res get
                    sb_assert(lua_isfunction(L, -1));
                    lua_remove(L, -2);          /// get
                    lua_pushvalue(L, 1);        /// get obj
                    lua_call(L, 1, 1);
                    return 1;
                }
				lua_pop(L, 2);                  /// mt 
                
                lua_pushliteral(L, "methods");
                lua_rawget(L, -2);                  /// mt methods
                sb_assert(lua_istable(L, -1));
                
                // Look for the key in the metatable
				lua_pushvalue(L, 2);
				lua_rawget(L, -2);              /// mt methods res
				// Did we get a non-nil result?  If so, return it
				if (!lua_isnil(L, -1)) {
                    lua_remove(L, -2);          /// mt res
                    lua_remove(L, -2);          /// res
                    sb_assert(lua_isfunction(L, -1));
                    return 1;
                }
				lua_pop(L, 2);                  /// mt 
                
                
				lua_pushliteral(L, "parent");
                lua_rawget(L, -2 );             /// mt prnt
				if (lua_isnil(L, -1)) {
					lua_pop(L, 2);
                    char buf[128];
                    lua_getglobal(L, "tostring");
                    lua_pushvalue(L, 1);
                    lua_call(L, 1, 1);
                    snprintf(buf, 128, "%s: get unknown field: %s",lua_tostring(L, -1),lua_tostring(L, 2));
                    lua_pop(L, 1);
                    lua_pushstring(L, buf);
                    lua_error(L);
                    return 0;
                }
				lua_remove(L, -2);              /// prnt
			} while (true);
			
			// Control never gets here
			return 0;
		}
        
        static int setter_indexer (lua_State *L)
		{
            sb_assert(lua_isuserdata(L, 1));
          	lua_getmetatable(L, 1);             /// mt
            do {
                lua_pushliteral(L, "props");
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
                    lua_call(L, 2, 0);
                    return 0;
                }
				lua_pop(L, 2);                  /// mt 
				lua_pushliteral(L, "parent");
                lua_rawget(L, -2 );             /// mt prnt
				if (lua_isnil(L, -1)) {
					lua_pop(L, 2);
                    char buf[128];
                    snprintf(buf, 128, "%s: set unknown field: %s",lua_tostring(L, 1),lua_tostring(L, 2));
                    lua_pushstring(L, buf);
                    lua_error(L);
                    return 0;
                }
				lua_remove(L, -2);              /// prnt
			} while (true);
			
			// Control never gets here
			return 0;
		}

        static int object_to_string( lua_State* L ) {
            if (lua_isuserdata(L, 1)) {
                data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, 1));
                void* res = 0;
                if ( holder->type == data_holder::raw_data ) {
                    res = holder+1;
                } else if ( holder->type == data_holder::raw_ptr ) {
                    res = *reinterpret_cast<void**>(holder+1);
                } else if ( holder->type == data_holder::shared_ptr ) {
                    res = *reinterpret_cast<void**>(holder+1); // hack
                }  
                char buf[128];
                snprintf(buf, 128, "object<%s>:%p",holder->info->name,res);
                lua_pushstring(L, buf);
                return 1;
            }
            return 0;
        }
        
        void LuaRegistrator::create_metatable(const meta::type_info* info) {
            lua_get_create_table( m_L, info->name );   /// mntbl
            sb_assert( lua_istable(m_L, -1));   
            lua_createtable(m_L, 0, 0);                     /// mntbl props
            lua_setfield(m_L, -2, "props");                 /// mntbl 
            lua_createtable(m_L, 0, 0);                     /// mntbl props
            lua_setfield(m_L, -2, "methods");                 /// mntbl 
            if (info->parent && info->parent!=meta::type<void>::info() ) {
                lua_get_create_table(m_L,info->parent->name);
                lua_setfield(m_L, -2, "parent");
            }
            lua_createtable(m_L, 0, 7);                     /// mntbl raw_ptr
            lua_pushcfunction(m_L, &destructor_func);               /// mntbl raw_ptr destructor_func
			lua_setfield(m_L, -2, "__gc");                  /// mntbl raw_ptr
            lua_pushcfunction(m_L, &getter_indexer);               /// mntbl raw_ptr indexer
			lua_setfield(m_L, -2, "__index");               /// mntbl raw_ptr
            lua_pushcfunction(m_L, &setter_indexer);               /// mntbl raw_ptr indexer
			lua_setfield(m_L, -2, "__newindex");               /// mntbl raw_ptr
            lua_pushcfunction(m_L, &object_to_string);               /// mntbl raw_ptr object_to_string
			lua_setfield(m_L, -2, "__tostring");               /// mntbl raw_ptr
            lua_getfield(m_L, -2, "props");                 /// mntbl raw_ptr props
            lua_setfield(m_L, -2, "props");                 /// mntbl raw_ptr
            lua_getfield(m_L, -2, "methods");                 /// mntbl raw_ptr methods
            lua_setfield(m_L, -2, "methods");                 /// mntbl raw_ptr
            lua_getfield(m_L, -2, "parent");                 /// mntbl raw_ptr parent
            lua_setfield(m_L, -2, "parent");                 /// mntbl raw_ptr
            lua_setfield(m_L, -2, "metatable");               /// mntbl 
            lua_pushvalue(m_L, -1);
            lua_setmetatable(m_L, -2);
        }


    };
    
}
