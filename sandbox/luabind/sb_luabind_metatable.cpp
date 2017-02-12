//
//  sb_luabind_metatable.cpp
//  YinYang
//
//  Created by Андрей Куницын on 7/15/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "sb_luabind_metatable.h"
#include "sb_luabind_stack.h"
#include "sb_inplace_string.h"
#include "../sb_log.h"

#include <sbstd/sb_platform.h>


namespace Sandbox {
    namespace luabind {
        static const char* const LuabindModule = "Sanbox:luabind";
        
                
        void lua_unregistered_error( lua_State* L, 
                                    const char* type ) {
            char buf[128];
            sb::snprintf(buf, 128, "unregistered type '%s'", type);

            LogError(LuabindModule) << buf ;

            lua_pushstring(L, buf);
            lua_error(L);
        }

        template <char C, size_t count>
        static void get_create_table_impl2(lua_State* L,const InplaceString& name,int prealloc) {
            InplaceString tail(name.begin(),name.end());
            while (!tail.empty()) {
                const char* other = tail.find(C);
                InplaceString head( tail.begin(), other );
                
                lua_pushlstring(L, head.begin(),head.length());
                sb_assert(lua_istable(L, -2));
                lua_rawget(L, -2);
                
                if (!lua_istable(L, -1)) {
                    lua_pop(L, 1);
                    lua_createtable(L, 0, other==name.end() ? prealloc : 1);
                    lua_pushlstring(L, head.begin(),head.length());
                    lua_pushvalue(L, -2);
                    lua_rawset(L, -4);
                }
                lua_remove(L, -2);
                if (other==name.end()) break;
                tail = InplaceString(other+count,name.end());
            }
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
            get_create_table_impl2<C,count>(L,tail,prealloc);
        }
        
        void lua_get_create_table(lua_State* L,const char* name,int prealloc) {
            get_create_table_impl<':',2>(L, InplaceString(name),prealloc);
        }

        static bool get_table(lua_State* L,const InplaceString& name, bool do_error = false) {
            const char* other = name.find(':');
            sb::string first = InplaceString(name.begin(),other).str();
            lua_getglobal(L,first.c_str());
            if (!lua_istable(L, -1)) {
                lua_pop(L, 1);
                if (do_error)
                    lua_unregistered_error( L ,name.begin() );
                return false;
            }
            if (*other==0) return true;
            other+=2;
            InplaceString tail(other,name.end());
            while (!tail.empty()) {
                other = tail.find(':');
                InplaceString head( tail.begin(), other );
                lua_getfield(L, -1, head.str().c_str() );
                if (!lua_istable(L, -1)) {
                    lua_pop(L, 2);
                    if (do_error)
                        lua_unregistered_error( L ,name.begin() );
                    return false;
                }
                lua_remove(L, -2);
                if (*other==0) break;
                tail = InplaceString(other+2,name.end());
            }
            return true;
        }
        
        void lua_set_metatable( lua_State* L, const data_holder& holder ) {
            if (lua_isnil(L, -1))
                return;
            get_table( L, InplaceString(holder.info->name) );   /// obj mntbl
            sb_assert(lua_istable(L, -1));
            lua_rawgeti(L, -1, mt_indexes::__metatable);   /// obj mntbl mt
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
        
        void lua_get_create_child_table( lua_State* L, const char*& path) {
            InplaceString full_path(path);
            const char* name = full_path.rfind('.');
            if (name==path) return;
            get_create_table_impl2<'.', 1>(L, InplaceString(path,name), 1);
            path = name+1;
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
        
        static bool lua_get_metatable( lua_State* L, const char* path) {
            InplaceString full_path(path);
            return get_table(L,full_path,false);
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
        
        static bool native_getter_indexer(lua_State* L,int self_index) {
            sb_assert(lua_isuserdata(L, self_index));
            /// debug
#if 0
            data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, self_index));
            if (strcmp(holder->info->name,"MyGUI::Widget")==0) {
                int a = 0;
            }
#endif

            lua_getmetatable(L, self_index);             /// mt
            do {
                lua_rawgeti(L, -1, mt_indexes::__props);                  /// mt props
                sb_assert(lua_istable(L, -1));
                
                // Look for the key in the metatable
				lua_pushvalue(L, 2);
                sb_assert(lua_isstring(L, -1));
                //LogDebug() << "index: " << lua_tostring(L, -1);
				lua_rawget(L, -2);              /// mt props res
				// Did we get a non-nil result?  If so, return it
				if (!lua_isnil(L, -1)) {
                    lua_remove(L, -2);          /// mt res
                    lua_remove(L, -2);          /// res
                    lua_rawgeti(L, -1, mt_indexes::__get); /// res get
                    if (!lua_isfunction(L, -1)) {
                        char buf[128];
                        sb::snprintf(buf, 128, "%s: get writeonly field: %s",lua_tostring(L, self_index),lua_tostring(L, 2));
                        lua_pushstring(L, buf);
                        lua_error(L);
                    }
                    sb_assert(lua_isfunction(L, -1));
                    lua_remove(L, -2);          /// get
                    lua_pushvalue(L, 1);        /// get obj
                    lua_call_method(L, 1, 1, "get");
                    return true;
                }
				lua_pop(L, 2);                  /// mt
                
                lua_rawgeti(L, -1,mt_indexes::__methods);                  /// mt methods
                sb_assert(lua_istable(L, -1));
                
                // Look for the key in the metatable
				lua_pushvalue(L, 2);
                sb_assert(lua_isstring(L, -1));
				lua_rawget(L, -2);              /// mt methods res
				// Did we get a non-nil result?  If so, return it
				if (!lua_isnil(L, -1)) {
                    lua_remove(L, -2);          /// mt res
                    lua_remove(L, -2);          /// res
                    sb_assert(lua_isfunction(L, -1));
                    return true;
                }
				lua_pop(L, 2);                  /// mt
                
                
				lua_rawgeti(L, -1, mt_indexes::__parent );             /// mt prnt
				if (lua_isnil(L, -1)) {
                    lua_pop(L, 2);
                    return false;
                }
				lua_remove(L, -2);              /// prnt
			} while (true);
			
			// Control never gets here
			return false;
        }
        
        static int getter_indexer (lua_State *L)
		{
            if (!native_getter_indexer(L,1)) {
                char buf[128];
                lua_getglobal(L, "tostring");
                lua_pushvalue(L, 1);
                lua_call(L, 1, 1);
                sb::snprintf(buf, 128, "%s: get unknown field: %s",lua_tostring(L, -1),lua_tostring(L, 2));
                lua_pop(L, 1);
                lua_pushstring(L, buf);
                lua_error(L);
                return 0;
            }
            return 1;
		}
        
        static bool native_setter_indexer(lua_State* L,int self_indx) {
            sb_assert(lua_isuserdata(L, self_indx));
          	lua_getmetatable(L, self_indx);             /// mt
            sb_assert(lua_istable(L, -1));
            do {
                lua_rawgeti(L, -1, mt_indexes::__props);                  /// mt props
                sb_assert(lua_istable(L, -1));
                
                // Look for the key in the metatable
				lua_pushvalue(L, 2);
				lua_rawget(L, -2);              /// mt props res
				// Did we get a non-nil result?  If so, return it
				if (!lua_isnil(L, -1)) {
                    lua_remove(L, -2);          /// mt res
                    lua_remove(L, -2);          /// res
                    lua_checkstack(L, 2);
                    lua_rawgeti(L, -1, mt_indexes::__set); /// res set
                    if (!lua_isfunction(L, -1)) {
                        char buf[128];
                        sb::snprintf(buf, 128, "%s: set readonly field: %s",lua_tostring(L, self_indx),lua_tostring(L, 2));
                        lua_pushstring(L, buf);
                        lua_error(L);
                    }
                    sb_assert(lua_isfunction(L, -1));
                    lua_remove(L, -2);          /// set
                    lua_pushvalue(L, 1);        /// set obj
                    lua_pushvalue(L, 3);        /// set obj val
                    lua_call_method(L, 2, 0, "set");
                    return true;
                }
				lua_pop(L, 2);                  /// mt
				lua_rawgeti(L, -1 , mt_indexes::__parent);             /// mt prnt
				if (lua_isnil(L, -1)) {
					lua_pop(L, 2);
                    return false;
                }
				lua_remove(L, -2);              /// prnt
			} while (true);
			
			// Control never gets here
			return false;
        }
        
        static int setter_indexer (lua_State *L)
		{
            LUA_CHECK_STACK(0);
            if (!native_setter_indexer(L,1)) {
                char buf[128];
                lua_getglobal(L, "tostring");
                
                sb::string object;
                if (lua_isfunction(L, -1)) {
                    lua_pushvalue(L, 1);
                    lua_call(L, 1, 1);
                    object = lua_tostring(L, -1);
                }
                lua_pop(L, 1);
                sb::snprintf(buf, 128, "%s: set unknown field: %s",object.c_str(),lua_tostring(L, 2));
                lua_pushstring(L, buf);
                lua_error(L);
            }
            return 0;
		}

        static const void* get_object_raw_ptr( data_holder* holder ) {
            void* res = 0;
            if ( holder->type == data_holder::raw_data ) {
                res = holder+1;
            } else if ( holder->type == data_holder::raw_ptr ) {
                res = *reinterpret_cast<void**>(holder+1);
            } else if ( holder->type == data_holder::shared_ptr ) {
                res = *(reinterpret_cast<void**>(holder+1)+1); // hack
            } else if ( holder->type == data_holder::intrusive_ptr ) {
                res = *reinterpret_cast<void**>(holder+1);
            }
            return res;
        }
        
        static int object_to_string( lua_State* L ) {
            if (lua_isuserdata(L, 1)) {
                data_holder* holder = reinterpret_cast<data_holder*>(lua_touserdata(L, 1));
                const void* res = get_object_raw_ptr(holder);
                char buf[128];
                sb::snprintf(buf, 128, "object<%s>:%p",holder->info->name,res);
                lua_pushstring(L, buf);
                return 1;
            }
            return 0;
        }
        
        static int object_compare( lua_State* L ) {
            if (lua_isuserdata(L, 1) && lua_isuserdata(L, 2)) {
                data_holder* holder1 = reinterpret_cast<data_holder*>(lua_touserdata(L, 1));
                data_holder* holder2 = reinterpret_cast<data_holder*>(lua_touserdata(L, 2));
                if (holder1->info != holder2->info) {
                    lua_pushboolean(L, 0);
                } else {
                    lua_pushboolean(L,get_object_raw_ptr(holder1) == get_object_raw_ptr(holder2) ? 1 : 0);
                }
            } else {
                lua_pushboolean(L, 0);
            }
            return 1;
        }

        
        void lua_create_metatable(lua_State* L) {
            lua_createtable(L, 5, 7);
            sb_assert( lua_istable(L, -1));   
            lua_createtable(L, 0, 0);                     /// mntbl props
            lua_rawseti(L, -2, mt_indexes::__props);                 /// mntbl
            lua_createtable(L, 0, 0);                     /// mntbl props
            lua_rawseti(L, -2, mt_indexes::__methods);                 /// mntbl
            lua_createtable(L, 5, 7);                     /// mntbl raw_ptr
            lua_pushcfunction(L, &destructor_func);               /// mntbl raw_ptr destructor_func
			lua_setfield(L, -2, "__gc");                  /// mntbl raw_ptr
            lua_pushcfunction(L, &getter_indexer);               /// mntbl raw_ptr indexer
			lua_setfield(L, -2, "__index");               /// mntbl raw_ptr
            lua_pushcfunction(L, &setter_indexer);               /// mntbl raw_ptr indexer
			lua_setfield(L, -2, "__newindex");               /// mntbl raw_ptr
            lua_pushcfunction(L, &object_to_string);               /// mntbl raw_ptr object_to_string
			lua_setfield(L, -2, "__tostring");               /// mntbl raw_ptr
            lua_pushcfunction(L, &object_compare);               /// mntbl raw_ptr object_to_string
            lua_setfield(L, -2, "__eq");               /// mntbl raw_ptr
            lua_rawgeti(L, -2, mt_indexes::__props);                 /// mntbl raw_ptr props
            lua_rawseti(L, -2, mt_indexes::__props);                 /// mntbl raw_ptr
            lua_rawgeti(L, -2, mt_indexes::__methods);                 /// mntbl raw_ptr methods
            lua_rawseti(L, -2, mt_indexes::__methods);                 /// mntbl raw_ptr
            lua_rawseti(L, -2, mt_indexes::__metatable);  /// mntbl
        }
        
        
        /// -2 - class-name
        /// -1 - class-metatable
        void lua_register_metatable(lua_State* L,const meta::type_info* info) {
            if (info->parent.info!=meta::type<void>::info() ) {
                if (!lua_get_metatable(L,info->parent.info->name)) {
                    LogError() << "unregistered parent " << info->parent.info->name;
                    sb_assert(false);
                    lua_pop(L, 1);
                    lua_get_create_table(L, info->parent.info->name, 2);
                }
                //lua_get_create_table(L,info->parent.info->name,2);  /// [metatatable] ptbl
                lua_rawseti(L, -2, mt_indexes::__parent);
                lua_rawgeti(L, -1, mt_indexes::__metatable);
                lua_rawgeti(L, -2, mt_indexes::__parent);                 /// mntbl raw_ptr parent
                lua_rawseti(L, -2, mt_indexes::__parent);                 /// mntbl raw_ptr
                lua_pop(L, 1);
            }
            
            lua_getfield(L, -1, "__constructor");
            if ( lua_isfunction(L, -1) ) {  /// mn fn
                lua_createtable(L, 0, 1);   /// mn fn mt
                lua_pushvalue(L, -2);       /// mn fn mt fn
                lua_setfield(L, -2, "__call");/// mn fn mt
                lua_setmetatable(L, -3);    /// mn fn
            } 
            lua_pop(L, 1);
            
            lua_set_metatable(L, info->name);
        }
        
        
        
        void lua_register_enum_metatable(lua_State* L,const meta::type_info* info,lua_CFunction compare) {
            sb_assert(info->parent.info==meta::type<void>::info());
            lua_rawgeti(L, -1, mt_indexes::__metatable); // mn mt
            lua_pushcclosure(L, compare, 0);    // mn mt cmp
            lua_setfield(L, -2,"__eq");         // mn mt
            lua_pop(L, 1);                      // mn
            lua_set_metatable(L, info->name);
        }
        
        
               
        
                
    
        
    }
}
