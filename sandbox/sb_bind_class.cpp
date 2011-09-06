/*
 *  sb_bind_class.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 09.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_bind_class.h"
#include "sb_assert.h"
#include "sb_bind_enum.h"

extern "C" {
#include "../lua/src/lua.h"
#include "../lua/src/lauxlib.h"
}

#include <string>

namespace Sandbox {

	namespace Bind {
		
		
		
		
		
		
		
		
		
		
		
		/*
		 * Index metamethod for C++ classes exposed to Lua.  This searches the
		 * metatable for the given key, but if it can't find it, it looks for a
		 * __parent key and delegates the lookup to that.
		 */
		
		static int indexer (lua_State *L)
		{
			lua_getmetatable(L, 1);
			
			do {
				// Look for the key in the metatable
				lua_pushvalue(L, 2);
				lua_rawget(L, -2);
				// Did we get a non-nil result?  If so, return it
				if (!lua_isnil(L, -1))
					return 1;
				lua_pop(L, 1);
				
				// Look for the key in the __propget metafield
				StackHelper::rawgetfield(L, -1, "__propget");
				if (!lua_isnil(L, -1))
				{
					lua_pushvalue(L, 2);
					lua_rawget(L, -2);
					// If we got a non-nil result, call it and return its value
					if (!lua_isnil(L, -1))
					{
						sb_assert(lua_isfunction(L, -1));
						lua_pushvalue(L, 1);
						lua_call(L, 1, 1);
						return 1;
					}
					lua_pop(L, 1);
				}
				lua_pop(L, 1);
				
				// Look for the key in the __const metafield
				StackHelper::rawgetfield(L, -1, "__const");
				if (!lua_isnil(L, -1))
				{
					lua_pushvalue(L, 2);
					lua_rawget(L, -2);
					if (!lua_isnil(L, -1))
						return 1;
					lua_pop(L, 1);
				}
				lua_pop(L, 1);
				
				// Look for a __parent metafield; if it doesn't exist, return nil;
				// otherwise, repeat the lookup on it.
				StackHelper::rawgetfield(L, -1, "__parent");
				if (lua_isnil(L, -1))
					return 1;
				lua_remove(L, -2);
			} while (true);
			
			// Control never gets here
			return 0;
		}
		
		/*
		 * Newindex metamethod for supporting properties on scopes and static
		 * properties of classes.
		 */
		
		static int newindexer (lua_State *L)
		{
			lua_getmetatable(L, 1);
			
			do {
				// Look for the key in the __propset metafield
				StackHelper::rawgetfield(L, -1, "__propset");
				if (!lua_isnil(L, -1))
				{
					lua_pushvalue(L, 2);
					lua_rawget(L, -2);
					// If we got a non-nil result, call it
					if (!lua_isnil(L, -1))
					{
						sb_assert(lua_isfunction(L, -1));
						lua_pushvalue(L, 3);
						lua_call(L, 1, 0);
						return 0;
					}
					lua_pop(L, 1);
				}
				lua_pop(L, 1);
				
				// Look for a __parent metafield; if it doesn't exist, error;
				// otherwise, repeat the lookup on it.
				StackHelper::rawgetfield(L, -1, "__parent");
				if (lua_isnil(L, -1))
				{
					return luaL_error(L, "attempt to set %s, which isn't a property",
									  lua_tostring(L, 2));
				}
				lua_remove(L, -2);
			} while (true);
			
			// Control never gets here
			return 0;
		}
		
		/*
		 * Newindex variant for properties of objects, which passes the
		 * object on which the property is to be set as the first parameter
		 * to the setter method.
		 */
		
		static int m_newindexer (lua_State *L)
		{
			lua_getmetatable(L, 1);
			
			do {
				// Look for the key in the __propset metafield
				StackHelper::rawgetfield(L, -1, "__propset");
				if (!lua_isnil(L, -1))
				{
					lua_pushvalue(L, 2);
					lua_rawget(L, -2);
					// If we got a non-nil result, call it
					if (!lua_isnil(L, -1))
					{
						sb_assert(lua_isfunction(L, -1));
						lua_pushvalue(L, 1);
						lua_pushvalue(L, 3);
						lua_call(L, 2, 0);
						return 0;
					}
					lua_pop(L, 1);
				}
				lua_pop(L, 1);
				
				// Look for a __parent metafield; if it doesn't exist, error;
				// otherwise, repeat the lookup on it.
				StackHelper::rawgetfield(L, -1, "__parent");
				if (lua_isnil(L, -1))
				{
					return luaL_error(L, "attempt to set %s, which isn't a property",
									  lua_tostring(L, 2));
				}
				lua_remove(L, -2);
			} while (true);
			
			// Control never gets here
			return 0;
		}
	
		
		static int lua_destructor_func(lua_State* L) {
			const ClassInfo* _class = reinterpret_cast<const ClassInfo*>(lua_touserdata(L, lua_upvalueindex(1)));
			ObjectData* obj = StackHelper::check_object_type(L,1,_class->name,false);
			if (obj) {
				if (obj->store_type==STORE_RAW) _class->destruct_raw(obj+1,L);
				if (obj->store_type==STORE_SHARED) _class->destruct_ptr(obj+1,L);
			}
			return 0;
		}
		
		
		/*
		 * Functions for metatable construction.  These functions create a metatable and
		 * leave it in the top element of the Lua stack (in addition to registering it
		 * wherever it needs to be registered).
		 */
		
		static void create_metatable (lua_State *L, const ClassBind* _class)
		{
			luaL_newmetatable(L, _class->info->name);
			// Set indexer as the __index metamethod
			lua_pushcfunction(L, &indexer);
			StackHelper::rawsetfield(L, -2, "__index");
			// Set m_newindexer as the __newindex metamethod
			lua_pushcfunction(L, &m_newindexer);
			StackHelper::rawsetfield(L, -2, "__newindex");
			// Set the __gc metamethod to call the class destructor
			lua_pushlightuserdata(L, const_cast<ClassInfo*>(_class->info));
			lua_pushcclosure(L, lua_destructor_func, 1);
			StackHelper::rawsetfield(L, -2, "__gc");
			// Set the __type metafield to the name of the class
			lua_pushstring(L, _class->info->name);
			StackHelper::rawsetfield(L, -2, "__type");
			lua_pushlightuserdata(L, const_cast<ClassBind*>(_class));
			StackHelper::rawsetfield(L, -2, "__bind");
			// Create the __propget and __propset metafields as empty tables
			lua_newtable(L);
			StackHelper::rawsetfield(L, -2, "__propget");
			lua_newtable(L);
			StackHelper::rawsetfield(L, -2, "__propset");
		}
		
		static void create_const_metatable (lua_State *L, const ClassBind* _class)
		{
			std::string constname = std::string("const ");
			constname += _class->info->name;
			luaL_newmetatable(L, constname.c_str());
			lua_pushcfunction(L, &indexer);
			StackHelper::rawsetfield(L, -2, "__index");
			lua_pushcfunction(L, &m_newindexer);
			StackHelper::rawsetfield(L, -2, "__newindex");
			lua_pushlightuserdata(L, const_cast<ClassInfo*>(_class->info));
			lua_pushcclosure(L, lua_destructor_func, 1);
			StackHelper::rawsetfield(L, -2, "__gc");
			lua_pushstring(L, constname.c_str());
			StackHelper::rawsetfield(L, -2, "__type");
			lua_pushlightuserdata(L, const_cast<ClassBind*>(_class));
			StackHelper::rawsetfield(L, -2, "__bind");
			lua_newtable(L);
			StackHelper::rawsetfield(L, -2, "__propget");
		}
		
		
		
		
		static void create_static_table (lua_State *L)
		{
			lua_newtable(L);
			
			// Set it as its own metatable
			lua_pushvalue(L, -1);
			lua_setmetatable(L, -2);
			
			// Set indexer as the __index metamethod
			lua_pushcfunction(L, &indexer);
			StackHelper::rawsetfield(L, -2, "__index");
			
			// Set newindexer as the __newindex metamethod
			lua_pushcfunction(L, &newindexer);
			StackHelper::rawsetfield(L, -2, "__newindex");
			
			// Create the __propget and __propset metafields as empty tables
			lua_newtable(L);
			StackHelper::rawsetfield(L, -2, "__propget");
			lua_newtable(L);
			StackHelper::rawsetfield(L, -2, "__propset");
		}
		/*
		 * Lookup a static table based on its fully qualified name, and leave it on
		 * the stack
		 */
		
		static void lookup_static_table (lua_State *L, const char *name)
		{
			lua_getglobal(L, "_G");
			bool is_first = true;
			if (name && name[0] != '\0')
			{
				std::string namestr(name);
				size_t start = 0, pos = 0;
				while ( (pos = namestr.find(':', start)) != std::string::npos) 
				{
					std::string tname = namestr.substr(start, pos - start);
					lua_getfield(L, -1, tname.c_str());
					if (lua_isnil(L,-1)) {
						lua_pop(L, 1);
						create_static_table(L);
						lua_pushvalue(L, -1);
						if (is_first)
							lua_setfield(L, -3, tname.c_str());
						else {
							StackHelper::rawsetfield(L, -3, tname.c_str());
						}

					}
					sb_assert(lua_istable(L, -1));
					lua_remove(L, -2);
					start = pos + 2;
					is_first = false;
				}
				std::string tname = namestr.substr(start);
				lua_getfield(L, -1, tname.c_str());
				if (lua_isnil(L,-1)) {
					lua_pop(L, 1);
					create_static_table(L);
					lua_pushvalue(L, -1);
					if (is_first)
						lua_setfield(L, -3, tname.c_str());
					else {
						StackHelper::rawsetfield(L, -3, tname.c_str());
					}
				} 
				sb_assert(lua_istable(L, -1));
				lua_remove(L, -2);
			}
		}
 
		static int raw_constructor_func(lua_State* L) {
			sb_assert(lua_islightuserdata(L,lua_upvalueindex(1)));
			const ConstructorInfo* constructor = reinterpret_cast<const ConstructorInfo*> (lua_touserdata(L, lua_upvalueindex(1)));
			StackHelper hpr(L,2,constructor->signature);
			constructor->raw(&hpr);
			return 1;
		}
		static int shared_constructor_func(lua_State* L) {
			sb_assert(lua_islightuserdata(L,lua_upvalueindex(1)));
			const ConstructorInfo* constructor = reinterpret_cast<const ConstructorInfo*> (lua_touserdata(L, lua_upvalueindex(1)));
			StackHelper hpr(L,2,constructor->signature);
			constructor->ptr(&hpr);
			return 1;
		}
		
		static int method_call_func(lua_State* L) {
			const ClassInfo* _class = reinterpret_cast<const ClassInfo*>(lua_touserdata(L, lua_upvalueindex(1)));
			const MethodInfo* method = reinterpret_cast<const MethodInfo*>(lua_touserdata(L, lua_upvalueindex(2)));
			void* obj = StackHelper::get_object_ptr(L,1,_class);
			StackHelper hpr(L,2,method->signature);
			method->call(obj,method->func.data,&hpr);
			return method->ret;
		}
		static int propget_func (lua_State *L)
		{
			const ClassInfo* _class = reinterpret_cast<const ClassInfo*>(lua_touserdata(L, lua_upvalueindex(1)));
			const PropertyInfo* property = reinterpret_cast<const PropertyInfo*>(lua_touserdata(L, lua_upvalueindex(2)));
			void* obj = StackHelper::get_object_ptr(L,1,_class);
			StackHelper hpr(L,2,property->type);
			property->get(obj,property->data.data,&hpr);
			return 1;
		}
		static int propset_func (lua_State *L)
		{
			const ClassInfo* _class = reinterpret_cast<const ClassInfo*>(lua_touserdata(L, lua_upvalueindex(1)));
			const PropertyInfo* property = reinterpret_cast<const PropertyInfo*>(lua_touserdata(L, lua_upvalueindex(2)));
			void* obj = StackHelper::get_object_ptr(L,1,_class);
			StackHelper hpr(L,2,property->type);
			property->set(obj,property->data_set.data,&hpr);
			return 0;
		}
		
		
		void register_type(lua_State* L,const ClassBind* bind) {
			int top = lua_gettop(L);
			(void)top;
			// Create metatable for this class.  The metatable is stored in the Lua
			// registry, keyed by the given class name.
			{
				create_metatable(L, bind);
			}
			sb_assert( lua_gettop(L)==(top+1) );
			if (bind->info->parent) {
				// Set the __parent metafield to the base class's metatable
				luaL_getmetatable(L, bind->info->parent);
				StackHelper::rawsetfield(L, -2, "__parent");
			}
			// Create const metatable for this class.  This is identical to the
			// previous metatable, except that it has "const " prepended to the __type
			// field, and has no __propset field.  Const methods will be added to the
			// const metatable, non-const methods to the normal metatable.
			{
				create_const_metatable(L, bind);
			}
			sb_assert( lua_gettop(L)==(top+2) );
			if (bind->info->parent) {
				std::string base_constname = std::string("const ");
				base_constname += bind->info->parent;
				luaL_getmetatable(L, base_constname.c_str());
				StackHelper::rawsetfield(L, -2, "__parent");
			}
			// Set __const metafield to point to the const metatable
			StackHelper::rawsetfield(L, -2, "__const");
			sb_assert( lua_gettop(L)==(top+1) );
			// Pop the original metatable
			lua_pop(L, 1);
			sb_assert( lua_gettop(L)==(top) );
			
			// Get a reference to the class's static table
			lookup_static_table(L, bind->info->name);
			if (bind->info->parent) {
				lookup_static_table(L,bind->info->parent);
				StackHelper::rawsetfield(L, -2, "__parent");
			}
			sb_assert( lua_gettop(L)==(top+1) );
			// Push the constructor proxy, with the constructor info as an upvalue
			lua_pushlightuserdata(L, const_cast<ConstructorInfo*>(bind->constructor));
			if (bind->constructor->raw) {
				lua_pushcclosure(L,raw_constructor_func, 1);
				StackHelper::rawsetfield(L, -2, "__call");
			} else if ( bind->constructor->ptr ) {
				lua_pushcclosure(L,shared_constructor_func, 1);
				StackHelper::rawsetfield(L, -2, "__call");
			} else {
				lua_pop(L,1);
			}

			lua_pop(L, 1);
			
			
			sb_assert(top==lua_gettop(L));
			
			for (const MethodInfo* method = bind->methods;method && method->name;method++) {
				std::string metatable_name = bind->info->name;
				luaL_getmetatable(L, metatable_name.c_str() );		/// MT
				if (method->is_const) {
					metatable_name.insert(0, "const ");
					luaL_getmetatable(L, metatable_name.c_str() );	/// MT CMT
				}
				
				lua_pushlightuserdata(L, const_cast<ClassInfo*>(bind->info));
				lua_pushlightuserdata(L, const_cast<MethodInfo*>(method) );
				lua_pushcclosure(L, &method_call_func, 2);			/// MT CMT C
				if (method->is_const) {			
					lua_pushvalue(L, -1);							/// MT CMT C C
					StackHelper::rawsetfield(L, -3, method->name);	/// MT CMT C
					StackHelper::rawsetfield(L, -3, method->name);	/// MT CMT
					lua_pop(L, 2);
				} else {
					StackHelper::rawsetfield(L, -2, method->name);
					lua_pop(L, 1);
				}
			}
			for (const PropertyInfo* property = bind->propertys;property && property->name;property++) {
				std::string metatable_name = bind->info->name;
				luaL_getmetatable(L, metatable_name.c_str() );	/// MT CMT
				metatable_name.insert(0, "const ");
				luaL_getmetatable(L, metatable_name.c_str() );		/// MT
				StackHelper::rawgetfield(L, -2, "__propget");
				StackHelper::rawgetfield(L, -2, "__propget");
								
				lua_pushlightuserdata(L, const_cast<ClassInfo*>(bind->info));
				lua_pushlightuserdata(L, const_cast<PropertyInfo*>(property) );
				lua_pushcclosure(L, &propget_func, 2);
				lua_pushvalue(L, -1);
				
				StackHelper::rawsetfield(L,-3,property->name);
				StackHelper::rawsetfield(L,-3,property->name);
				lua_pop(L,3);
				
				if (property->set) {
					StackHelper::rawgetfield(L, -1, "__propset");
					lua_pushlightuserdata(L, const_cast<ClassInfo*>(bind->info));
					lua_pushlightuserdata(L, const_cast<PropertyInfo*>(property) );
					lua_pushcclosure(L, &propset_func, 2);
					StackHelper::rawsetfield(L,-2,property->name);
					lua_pop(L,1);
				} 
				lua_pop(L,1);
				
			}
		}
		
		void register_enum(lua_State* L,const EnumBind* info) {
			lookup_static_table(L, info->name);
			for (const EnumItem* item = info->items;item->name;item++) {
				lua_pushnumber(L, item->value);
				StackHelper::rawsetfield(L,-2,item->name);
			}
			lua_pop(L,1);
		}
		
	}
}
