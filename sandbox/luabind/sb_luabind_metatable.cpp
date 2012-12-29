//
//  sb_luabind_metatable.cpp
//  YinYang
//
//  Created by Андрей Куницын on 7/15/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "sb_luabind_metatable.h"
#include "sb_luabind_stack.h"
#include "sb_luabind_wrapper.h"
#include "sb_inplace_string.h"
#include "../sb_log.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

namespace Sandbox {
    namespace luabind {
        static const char* const LuabindModule = "Sanbox:luabind";
        
        void lua_unregistered_error( lua_State* L, 
                                    const char* type ) {
            char buf[128];
            ::snprintf(buf, 128, "unregistered type '%s'", type);

            LogError(LuabindModule) << buf ;

            lua_pushstring(L, buf);
            lua_error(L);
        }

        template <char C, size_t count>
        static void get_create_table_impl(lua_State* L,const InplaceString& name,int prealloc) {
            const char* other = name.find(C);
            sb::string first = InplaceString(name.begin(),other).str();
            lua_getglobal(L,first.c_str());
            if (!lua_istable(L, -1)) {
                lua_pop(L, 1);
                lua_createtable(L, 0, prealloc);
                lua_pushvalue(L, -1);
                lua_setglobal(L, first.c_str());
            }
            if (other==name.end()) return;
            other+=count;
            if (other >= name.end()) return;
            InplaceString tail(other,name.end());
            while (!tail.empty()) {
                other = tail.find(C);
                InplaceString head( tail.begin(), other );
                lua_getfield(L, -1, head.str().c_str() );
                if (!lua_istable(L, -1)) {
                    lua_pop(L, 1);
                    lua_createtable(L, 0, other==name.end() ? prealloc : 1);
                    lua_pushvalue(L, -1);
                    lua_setfield(L, -3, head.str().c_str());
                }
                lua_remove(L, -2);
                if (other==name.end()) break;
                tail = InplaceString(other+count,name.end());
            }
        }
        
        void lua_get_create_table(lua_State* L,const char* name,int prealloc) {
            get_create_table_impl<':',2>(L, InplaceString(name),prealloc);
        }

        static void get_table(lua_State* L,const InplaceString& name) {
            const char* other = name.find(':');
            sb::string first = InplaceString(name.begin(),other).str();
            lua_getglobal(L,first.c_str());
            if (!lua_istable(L, -1)) {
                lua_pop(L, 1);
                lua_unregistered_error( L ,name.begin() );
                return;
            }
            if (*other==0) return;
            other+=2;
            InplaceString tail(other,name.end());
            while (!tail.empty()) {
                other = tail.find(':');
                InplaceString head( tail.begin(), other );
                lua_getfield(L, -1, head.str().c_str() );
                if (!lua_istable(L, -1)) {
                    lua_pop(L, 2);
                    lua_unregistered_error( L ,name.begin() );
                    return;
                }
                lua_remove(L, -2);
                if (*other==0) break;
                tail = InplaceString(other+2,name.end());
            }
        }
        
        void lua_set_metatable( lua_State* L, const data_holder& holder ) {
            if (lua_isnil(L, -1))
                return;
            get_table( L, InplaceString(holder.info->name) );   /// obj mntbl
            sb_assert(lua_istable(L, -1));
            lua_pushliteral(L, "__metatable");
            lua_rawget(L, -2);                          /// obj mntbl mt
            sb_assert(lua_istable(L, -1));
            lua_setmetatable(L, -3);                    /// obj mntbl
            lua_pop(L, 1);
        }
        
        void lua_set_value( lua_State* L, const char* path ) {
            InplaceString full_path(path);
            const char* name = full_path.rfind('.');
            if ( name == path ) {
                lua_setglobal(L, path);
            } else {
                get_create_table_impl<'.',1>(L, InplaceString(path,name),1);  /// val tbl
                lua_pushvalue(L, -2);               /// val tbl val
                lua_setfield(L, -2, name+1);        /// val tbl
                lua_pop(L, 2);
            }
        }

        
        static void lua_set_metatable( lua_State* L, const char* path ) {
            InplaceString full_path(path);
            const char* name = full_path.rfind(':');
            if ( name == path ) {
                lua_setglobal(L, path);
            } else {
                name--;
                get_create_table_impl<':',2>(L, InplaceString(path,name),1);  /// val tbl
                lua_pushvalue(L, -2);               /// val tbl val
                lua_setfield(L, -2, name+2);        /// val tbl
                lua_pop(L, 2);
            }
        }
        
        static int destructor_func( lua_State* L ) {
            if ( lua_isuserdata(L, 1 ) ) {
                data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, 1));
                if ( holder->destructor ) {
                    holder->destructor( holder );
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
            if (strcmp(holder->info->name,"Sandbox::TouchInfo")==0) {
                int a = 0;
            }
#endif
          	lua_getmetatable(L, 1);             /// mt
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
                    lua_getfield(L, -1, "get"); /// res get
                    sb_assert(lua_isfunction(L, -1));
                    lua_remove(L, -2);          /// get
                    lua_pushvalue(L, 1);        /// get obj
                    lua_call_method(L, 1, 1, "get");
                    return 1;
                }
				lua_pop(L, 2);                  /// mt 
                
                lua_pushliteral(L, "__methods");
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
                
                
				lua_pushliteral(L, "__parent");
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
                    lua_call_method(L, 2, 0, "set");
                    return 0;
                }
				lua_pop(L, 2);                  /// mt 
				lua_pushliteral(L, "__parent");
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
                    res = *(reinterpret_cast<void**>(holder+1)+1); // hack
                } else if ( holder->type == data_holder::wrapper_ptr ) {
                    res = *(reinterpret_cast<void**>(holder+1)+2); // hack
                }  
                char buf[128];
                snprintf(buf, 128, "object<%s>:%p",holder->info->name,res);
                lua_pushstring(L, buf);
                return 1;
            }
            return 0;
        }
        
        void lua_create_metatable(lua_State* L) {
            lua_createtable(L, 0, 7);
            sb_assert( lua_istable(L, -1));   
            lua_createtable(L, 0, 0);                     /// mntbl props
            lua_setfield(L, -2, "__props");                 /// mntbl 
            lua_pushboolean(L, 1);                     
            lua_setfield(L, -2, "__isnative");                 /// mntbl 
            lua_createtable(L, 0, 0);                     /// mntbl props
            lua_setfield(L, -2, "__methods");                 /// mntbl 
            lua_createtable(L, 0, 7);                     /// mntbl raw_ptr
            lua_pushcfunction(L, &destructor_func);               /// mntbl raw_ptr destructor_func
			lua_setfield(L, -2, "__gc");                  /// mntbl raw_ptr
            lua_pushcfunction(L, &getter_indexer);               /// mntbl raw_ptr indexer
			lua_setfield(L, -2, "__index");               /// mntbl raw_ptr
            lua_pushcfunction(L, &setter_indexer);               /// mntbl raw_ptr indexer
			lua_setfield(L, -2, "__newindex");               /// mntbl raw_ptr
            lua_pushcfunction(L, &object_to_string);               /// mntbl raw_ptr object_to_string
			lua_setfield(L, -2, "__tostring");               /// mntbl raw_ptr
            lua_getfield(L, -2, "__props");                 /// mntbl raw_ptr props
            lua_setfield(L, -2, "__props");                 /// mntbl raw_ptr
            lua_getfield(L, -2, "__methods");                 /// mntbl raw_ptr methods
            lua_setfield(L, -2, "__methods");                 /// mntbl raw_ptr
            lua_setfield(L, -2, "__metatable");               /// mntbl 
        }
        
        static int init_func( lua_State* L ) {
            int args_cnt = lua_gettop(L); 
            lua_pushliteral(L, "__native");
            lua_pushvalue(L, lua_upvalueindex(1));
            for (int i=1;i<=args_cnt;i++) {
                lua_pushvalue(L, i);
            }
            lua_call_method(L, args_cnt, 1,"init_func");
            lua_rawset(L, 1);
            return 0;
        }
        
        void lua_register_metatable(lua_State* L,const meta::type_info* info) {
            if (info->parents && info->parents[0].info!=meta::type<void>::info() ) {
                lua_get_create_table(L,info->parents[0].info->name,2);
                lua_setfield(L, -2, "__parent");
                lua_getfield(L, -1, "__metatable");
                lua_getfield(L, -2, "__parent");                 /// mntbl raw_ptr parent
                lua_setfield(L, -2, "__parent");                 /// mntbl raw_ptr
                lua_pop(L, 1);
            }
            
            lua_getfield(L, -1, "__constructor");
            if ( lua_isfunction(L, -1) ) {  /// mn fn
                lua_pushvalue(L, -1);       /// mn fn fn
                lua_pushcclosure(L, &init_func, 1); /// mn fn if
                lua_setfield(L, -3, "__init");  /// mn fn
                lua_createtable(L, 0, 1);   /// mn fn mt
                lua_pushvalue(L, -2);       /// mn fn mt fn
                lua_setfield(L, -2, "__call");/// mn fn mt
                lua_setmetatable(L, -3);    /// mn fn
            } 
            lua_pop(L, 1);
            
            lua_set_metatable(L, info->name);
        }
        
        
        
        void lua_register_enum_metatable(lua_State* L,const meta::type_info* info,lua_CFunction compare) {
            sb_assert(info->parents[0].info==meta::type<void>::info());
            lua_getfield(L, -1, "__metatable"); // mn mt
            lua_pushcclosure(L, compare, 0);    // mn mt cmp
            lua_setfield(L, -2,"__eq");         // mn mt
            lua_pop(L, 1);                      // mn
            lua_set_metatable(L, info->name);
        }
        
        
        static int wrapper_init_func( lua_State* L ) {
            int args_cnt = lua_gettop(L); 
            lua_pushvalue(L, lua_upvalueindex(1));
            for (int i=1;i<=args_cnt;i++) {
                lua_pushvalue(L, i);
            }
            lua_call_method(L, args_cnt, 1, "wrapper_init_func");
            sb_assert(lua_isuserdata(L, -1));
            wrapper_holder* wh = reinterpret_cast<wrapper_holder*>(lua_touserdata(L, -1));
            sb_assert(wh->type==data_holder::wrapper_ptr);
            wrapper* w = get_shared_ptr<wrapper>(wh->info, wh+1).get();
            sb_assert(w);
            lua_pushliteral(L, "__native");
            lua_pushvalue(L, -2);
            lua_rawset(L, 1);
            lua_pop(L, 1);
            lua_pushvalue(L, 1);
            w->SetObject(L);
            wh->unlock(wh);
            return 0;
        }
        
        void lua_register_wrapper(lua_State* L,const meta::type_info* info) {
            lua_get_create_table(L,info->parents[0].info->name,2);
            lua_setfield(L, -2, "__parent");
            lua_getfield(L, -1, "__metatable");
            lua_getfield(L, -2, "__parent");                 /// mntbl raw_ptr parent
            lua_setfield(L, -2, "__parent");                 /// mntbl raw_ptr
            lua_pop(L, 1);
            
            lua_getfield(L, -1, "__constructor");
            if ( lua_isfunction(L, -1) ) {  /// mn fn
                lua_pushcclosure(L, &wrapper_init_func, 1); /// mn if
                lua_setfield(L, -2, "__init");  /// mn fn
            } else {
                lua_pop(L, 1);
                sb_assert( false );
            }
            lua_getfield(L, -1, "__parent");
            lua_getfield(L, -2, "__init");
            lua_setfield(L, -2, "__init");
            lua_getfield(L, -2, "__methods");
            lua_pushnil(L);
            while (lua_next(L, -2) != 0) {
                /* uses 'key' (at index -2) and 'value' (at index -1) */
                if (lua_isstring(L, -2))
                    lua_setfield(L, -4, lua_tostring(L, -2));
                else
                /* removes 'value'; keeps 'key' for next iteration */
                    lua_pop(L, 1);
            }
            lua_pop(L, 2);
            
            lua_set_metatable(L, info->name);
        }
        
        
        static int lua_class_constructor_func( lua_State* L ) {
            int args_cnt = lua_gettop(L);
            
            lua_newtable(L);
      
            lua_pushliteral(L, "__metatable");
            lua_rawget(L, 1);
            lua_setmetatable(L, -2);
            
            lua_pushliteral(L, "__init");
            lua_rawget(L, 1);
            if ( lua_isfunction(L, -1) ) {
                lua_pushvalue(L, -2);
                for (int i=2;i<=args_cnt;i++) {
                    lua_pushvalue(L, i);
                }
                lua_call_method(L, args_cnt, 0, "__init");
            } else {
                /// @todo : call parent instead
                lua_pop(L, 1);
            }
            
            return 1;
        }
        
        static int lua_getter_indexer (lua_State *L)
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
        
        static int lua_setter_indexer (lua_State *L)
		{
            sb_assert(lua_istable(L, 1));
            lua_pushliteral(L, "__native");
            lua_rawget(L, 1);
            if (lua_isuserdata(L, -1)) {
                
#if 0
                data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, -1));
                if (strcmp(holder->info->name,"Sandbox::Sprite")==0) {
                    int a = 0;
                }
#endif
                lua_getmetatable(L, -1);             /// mt
                sb_assert(lua_istable(L, -1));
                lua_remove(L, -2);
                do {
                    lua_pushliteral(L, "__props");
                    lua_rawget(L, -2);                  /// mt props
                    if (!lua_istable(L, -1)) {
                        lua_argerror(L,1,"noraml",0);
                    }
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
        
        static int lua_object_to_string( lua_State* L ) {
            if (lua_istable(L, 1)) {
                lua_pushliteral(L, "__native");
                lua_rawget(L, 1);
                char buf[128];
                if (lua_isuserdata(L, -1)) {
                    data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, -1));
                    void* res = 0;
                    if ( holder->type == data_holder::raw_data ) {
                        res = holder+1;
                    } else if ( holder->type == data_holder::raw_ptr ) {
                        res = *reinterpret_cast<void**>(holder+1);
                    } else if ( holder->type == data_holder::shared_ptr ) {
                        res = *(reinterpret_cast<void**>(holder+1)+1); // hack
                    } else if ( holder->type == data_holder::wrapper_ptr ) {
                        res = *(reinterpret_cast<void**>(holder+1)+2); // hack
                    }  
                    snprintf(buf, 128, "lua_object<%s(%s:%p)>:%p",
                             lua_tostring(L,lua_upvalueindex(1)),
                             holder->info->name,res,lua_topointer(L, 1));
                } else {
                    snprintf(buf, 128, "lua_object<%s>:%p",
                             lua_tostring(L,lua_upvalueindex(1)),
                             lua_topointer(L, 1));
                }
                lua_pop(L, 1);
                lua_pushstring(L, buf);
                return 1;
            }
            return 0;
        }
        
        
        int lua_class_func( lua_State* L ) {
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
            lua_pushcfunction(L, &lua_getter_indexer);               /// mntbl raw_ptr indexer
			lua_setfield(L, -2, "__index");               /// mntbl raw_ptr
            lua_pushcfunction(L, &lua_setter_indexer);               /// mntbl raw_ptr indexer
			lua_setfield(L, -2, "__newindex");               /// mntbl raw_ptr
            lua_pushvalue(L, 1);
            lua_pushcclosure(L, &lua_object_to_string,1);               /// mntbl raw_ptr object_to_string
			lua_setfield(L, -2, "__tostring");               /// mntbl raw_ptr
            
            lua_pushvalue(L, -2);
            lua_setfield(L, -2, "__class");
            lua_setfield(L, -2, "__metatable");
            
            lua_pushvalue(L, -1);
            lua_setglobal(L, name);
            lua_pushcclosure(L, &lua_class_superclass_func, 1);
            return 1;
        }

    }
}
