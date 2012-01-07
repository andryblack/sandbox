/*
 *  sb_bind_stack.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 09.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_bind_stack.h"
#include "sb_bind_class.h"
#include "sb_log.h"

extern "C" {
#include "../lua/src/lua.h"
#include "../lua/src/lauxlib.h"
}

#ifdef _MSC_VER
#define snprintf _snprintf
#endif


namespace Sandbox {
	namespace Bind {
        
        static const char* MODULE = "Sandbox:Bind";
        
		static inline InplaceString get_ret_type(const char* sign) {
			InplaceString res(sign);
			return InplaceString(res.begin(),res.find('('));
		}
		static inline InplaceString strip_spaces( const InplaceString& str ) {
			const char* beg = str.begin();
			const char* end = str.end();
			while (::isspace(*beg)) beg++;
			while (end>beg && ::isspace(*(end-1))) end--;
			return InplaceString( beg,end);
		}
		InplaceString StackHelper::get_arg_type(int num) const {
			InplaceString res(m_signature);
			res = InplaceString(res.find('('),res.rfind(')'));
			if (*res.begin()=='(') res = InplaceString(res.begin()+1,res.end());
			for (int i=0;i<num;i++) {
				res = InplaceString(res.find(','),res.end());
				if (*res.begin()==',') res = InplaceString(res.begin()+1,res.end());
			}
			return strip_spaces(InplaceString(res.begin(),res.find(',')));
		}
		
		// Convenience functions: like lua_getfield and lua_setfield, but raw
		void StackHelper::rawgetfield (lua_State *L, int idx, const char *key)
		{
			lua_pushstring(L, key);
			if (idx < 0) --idx;
			lua_rawget(L, idx);
		}
		void StackHelper::rawsetfield (lua_State *L, int idx, const char *key)
		{
			lua_pushstring(L, key);
			lua_insert(L, -2);
			if (idx < 0) --idx;
			lua_rawset(L, idx);
		}
		
		ObjectData*  StackHelper::check_object_type(lua_State* L,int indx,const char* type,bool derived) {
			
			// If idx is relative to the top of the stack, convert it into an index
			// relative to the bottom of the stack, so we can push our own stuff
			if (indx < 0)
				indx += lua_gettop(L) + 1;
			
			// Check that the thing on the stack is indeed a userdata
            
            if (!lua_isuserdata(L, indx)) {
                lua_pushvalue(L, indx);
                while (lua_istable(L, -1)) {
                    rawgetfield(L, -1, "__parent");
                    lua_remove(L, -2);
                    if ( lua_isuserdata(L, -1) )  {
                        ObjectData* data = check_object_type(L,-1,type,derived);
                        lua_pop(L, 1);
                        return data;
                    }
                }
                lua_pop(L, 1);
                luaL_typerror(L, indx, type);
                return 0;
            }
            
			
			// Lookup the given name in the registry
			luaL_getmetatable(L, type);
			sb_assert(lua_istable(L,-1));
			// Lookup the metatable of the given userdata
			lua_getmetatable(L, indx);
			sb_assert(lua_istable(L,-1));
			
			const char* tname = type;
			
			// If exact match required, simply test for identity.
			if (!derived)
			{
				// Ignore "const" for exact tests (which are used for destructors).
				/*if (!strncmp(tname, "const ", 6))
					tname += 6;*/
				
				if (lua_rawequal(L, -1, -2))
					return reinterpret_cast<ObjectData*>(lua_touserdata(L, indx));
				else
				{
					// Generate an informative error message
					rawgetfield(L, -1, "__type");
					rawgetfield(L, -3, "__type");
					char buffer[256];
					snprintf(buffer, 256, "%s expected, got %s", lua_tostring(L, -1),
							 lua_tostring(L, -2));
					// luaL_argerror does not return
					luaL_argerror(L, indx, buffer);
					return 0;
				}
			}
			
			// Navigate up the chain of parents if necessary
			while (!lua_rawequal(L, -1, -2))
			{
				// Check for equality to the const metatable
				rawgetfield(L, -1, "__const");
				if (!lua_isnil(L, -1))
				{
					if (lua_rawequal(L, -1, -3))
						break;
				}
				lua_pop(L, 1);
				
				// Look for the metatable's parent field
				rawgetfield(L, -1, "__parent");
				
				// No parent field?  We've failed; generate appropriate error
				if (lua_isnil(L, -1))
				{
					// Lookup the __type field of the original metatable, so we can
					// generate an informative error message
					lua_getmetatable(L, indx);
					rawgetfield(L, -1, "__type");
					
					char buffer[256];
					snprintf(buffer, 256, "%s expected, got %s", tname,
							 lua_tostring(L, -1));
					// luaL_argerror does not return
					luaL_argerror(L, indx, buffer);
					return 0;
				}
				
				// Remove the old metatable from the stack
				lua_remove(L, -2);
			}
			
			// Found a matching metatable; return the userdata
			return reinterpret_cast<ObjectData*>(lua_touserdata(L, indx));
			
		}
		bool StackHelper::is_null(int indx) const {
			return lua_isnil(m_L,indx+m_base_index)!=0;
		}
		bool StackHelper::GetArgument(int indx,const ArgumentTag<bool>& ) const {
			return lua_toboolean(m_L, indx+m_base_index)!=0;
		}
		int StackHelper::GetArgument(int indx,const ArgumentTag<int>& ) const {
			return int(lua_tonumber(m_L, indx+m_base_index));
		}
		GHL::UInt32 StackHelper::GetArgument(int indx,const ArgumentTag<GHL::UInt32>& ) const {
			return GHL::UInt32(lua_tonumber(m_L, indx+m_base_index));
		}
#ifndef _MSC_VER
		size_t StackHelper::GetArgument(int indx,const ArgumentTag<size_t>& ) const {
			return size_t(lua_tonumber(m_L, indx+m_base_index));
		}
#endif
		float StackHelper::GetArgument(int indx,const ArgumentTag<float>& ) const {
			return float(lua_tonumber(m_L, indx+m_base_index));
		}
		const char* StackHelper::GetArgument(int indx,const ArgumentTag<const char*>& ) const {
			return get_string(indx);
		}
		const char* StackHelper::get_string(int indx) const {
			return lua_tostring(m_L,indx+m_base_index);
		}
		void StackHelper::push_string(const char* str) const {
			lua_pushstring(m_L, str);
		}
		void StackHelper::PushValue(int v) const {
			lua_pushnumber(m_L, v);
		}
		void StackHelper::PushValue(GHL::UInt32 v) const {
			lua_pushnumber(m_L, v);
		}
#ifndef _MSC_VER
		void StackHelper::PushValue(size_t v) const {
			lua_pushnumber(m_L, v);
		}
#endif
		void StackHelper::PushValue(float v) const {
			lua_pushnumber(m_L, v);
		}
		void StackHelper::PushValue(const char* str) const {
			lua_pushstring(m_L, str);
		}
		void StackHelper::PushValue(const std::string& str) const {
			lua_pushstring(m_L, str.c_str());
		}
		void StackHelper::PushValue(const InplaceString& v) const {
			lua_pushlstring(m_L,v.begin(),v.length());
		}
		void StackHelper::PushValue(bool v) const {
			lua_pushboolean(m_L, v);
		}
				
		void* StackHelper::get_object_ptr(lua_State* L,int indx,const ClassInfo* _class) {
			ObjectData* data = check_object_type(L,indx,_class->name,true);
			if (data) {
				if (data->store_type == STORE_RAW) return data+1;
				if (data->store_type == STORE_RAW_PTR) return *reinterpret_cast<void**>(data+1);
				if (data->store_type == STORE_SHARED) if (_class->get_ptr) return _class->get_ptr(data+1);
			}
			return 0;
		}
		void StackHelper::push_null() const {
			lua_pushnil(m_L);
		}
		void StackHelper::push_object_ptr(void* v) const {
			lua_State* L = m_L;
			std::string type = get_ret_type(m_signature).str();
			ObjectData *block = reinterpret_cast<ObjectData*>(lua_newuserdata(L, sizeof(ObjectData)+sizeof(void*)));
			block->store_type = STORE_RAW_PTR;
			*reinterpret_cast<void**>(block+1) = v;
			/// getting metatable
			luaL_getmetatable(L, type.c_str());
			if (!lua_istable(L,-1)) {
				lua_pop(L,1);
				luaL_error(L, "attempt to push ptr to unknown type %s",
						   type.c_str());
				return;
			}
			lua_setmetatable(L, -2);
		}
        void StackHelper::push_object_ptr(const void* v) const {
            lua_State* L = m_L;
			std::string type = "const ";
            type+=get_ret_type(m_signature).str();
			ObjectData *block = reinterpret_cast<ObjectData*>(lua_newuserdata(L, sizeof(ObjectData)+sizeof(void*)));
			block->store_type = STORE_RAW_PTR;
			*reinterpret_cast<const void**>(block+1) = v;
			/// getting metatable
			luaL_getmetatable(L, type.c_str());
			if (!lua_istable(L,-1)) {
				lua_pop(L,1);
				luaL_error(L, "attempt to push ptr to unknown type const %s",
						   type.c_str());
				return;
			}
			lua_setmetatable(L, -2);
        }
		void* StackHelper::new_object_shared_ptr() const {
			lua_State* L = m_L;
			std::string type = get_ret_type(m_signature).str();
			/// getting metatable
			luaL_getmetatable(L, type.c_str());
			if (!lua_istable(L,-1)) {
				lua_pop(L,1);
				luaL_error(L, "attempt to push shared_ptr to unknown type %s",
						   type.c_str());
				return 0;
			}
			lua_pushstring(L, "__bind");
			 lua_rawget(L, -2);
			 sb_assert(lua_islightuserdata(L,-1));
			 const ClassInfo* bind = reinterpret_cast<const ClassInfo*> (lua_touserdata(L, -1));
			 lua_pop(L,1);
			 
			ObjectData *block = reinterpret_cast<ObjectData*>(lua_newuserdata(L, sizeof(ObjectData)+bind->ptr_size));
			block->store_type = STORE_SHARED;
			lua_pushvalue(L, -2);
			lua_setmetatable(L, -2);
			lua_remove(L, -2);
			return (block+1);
		} 
		void* StackHelper::new_object_raw() const {
			lua_State* L = m_L;
			std::string type = get_ret_type(m_signature).str();
			/// getting metatable
			luaL_getmetatable(L, type.c_str());
			if (!lua_istable(L,-1)) {
				lua_pop(L,1);
				luaL_error(L, "attempt to push raw to unknown type %s",
						   type.c_str());
				return 0;
			}
			lua_pushstring(L, "__bind");
			lua_rawget(L, -2);
			sb_assert(lua_islightuserdata(L,-1));
			const ClassInfo* bind = reinterpret_cast<const ClassInfo*> (lua_touserdata(L, -1));
			lua_pop(L,1);
			
			ObjectData *block = reinterpret_cast<ObjectData*>(lua_newuserdata(L, sizeof(ObjectData)+bind->obj_size));
			block->store_type = STORE_RAW;
			lua_pushvalue(L, -2);
			lua_setmetatable(L, -2);
			lua_remove(L, -2);
			return block+1;
		}
		void* StackHelper::get_ptr(int indx) const {
			std::string type = get_arg_type(indx).str();
			ObjectData* data = check_object_type(m_L, indx+m_base_index, type.c_str(), false); /// @todo
			if (data->store_type == STORE_RAW) return data+1;
			if (data->store_type == STORE_RAW_PTR) return *reinterpret_cast<void**>(data+1);
			/// @todo shared
			return 0;
		}
        static void get_shared_ptr_impl(lua_State* L,int indx,void* to, const std::string& type) {
            ObjectData* data = reinterpret_cast<ObjectData*>(lua_touserdata(L, indx));
			if (data->store_type != STORE_SHARED) {
				luaL_typerror(L, indx, type.c_str());
				return;
			}
			// Lookup the given name in the registry
			luaL_getmetatable(L, type.c_str());
			sb_assert(lua_istable(L,-1));
			// Lookup the metatable of the given userdata
			lua_getmetatable(L, indx>0 ? indx : indx-1);
			sb_assert(lua_istable(L,-1));
			lua_pushstring(L, "__bind");
			lua_rawget(L, -2);
			sb_assert(lua_islightuserdata(L,-1));
			const ClassInfo* bind = reinterpret_cast<const ClassInfo*> (lua_touserdata(L, -1));
			lua_pop(L,1);
			const char* getted_name = bind->name;
			static const size_t buff_size = sizeof(shared_ptr<int>);
			sb_assert(buff_size>=bind->ptr_size);
			char buf1[buff_size];
			char buf2[buff_size];
			void* ptr_data = data+1;
			void* to_ptr = buf1;
			void* cp_ptr = 0;
			while (true) {
				if (lua_rawequal(L, -1, -2)) {
					bind->copy_ptr(ptr_data,to);
					if (cp_ptr) {
						bind->destruct_ptr(cp_ptr,0);
					}
					lua_pop(L,2);
					return;
				}
				// Look for the metatable's parent field
				StackHelper::rawgetfield(L, -1, "__parent");
				if (!lua_istable(L, -1))
				{
					lua_pop(L,1);
					char buffer[256];
					snprintf(buffer, 256, "%s expected, got %s", type.c_str(),
							 getted_name);
					// luaL_argerror does not return
					luaL_argerror(L, indx, buffer);
					return;
				}
				bind->cast_ptr(ptr_data,to_ptr);
				ptr_data = to_ptr;
				if (to_ptr == buf1) to_ptr = buf2; else to_ptr=buf1;
				if (cp_ptr) {
					bind->destruct_ptr(cp_ptr,0);
				}
				cp_ptr = ptr_data;
				lua_remove(L, -2);
				lua_pushstring(L, "__bind");
				lua_rawget(L, -2);
				sb_assert(lua_islightuserdata(L,-1));
				bind = reinterpret_cast<const ClassInfo*> (lua_touserdata(L, -1));
				lua_pop(L,1);
			}
        }
		void StackHelper::get_shared_ptr(int indx,void* to) const {
			std::string type = get_arg_type(indx).str();
			
            indx+=m_base_index;
			lua_State* L = m_L;
            if (!lua_isuserdata(L, indx)) {
                lua_pushvalue(L, indx);
                while (lua_istable(L, -1)) {
                    rawgetfield(L, -1, "__parent");
                    lua_remove(L, -2);
                    if ( lua_isuserdata(L, -1) )  {
                        get_shared_ptr_impl(L,-1, to,type);
                        lua_pop(L, 1);
                        return;
                    }
                }
                lua_pop(L, 1);
                luaL_typerror(L, indx, type.c_str());
                return;
            }
            get_shared_ptr_impl(L,indx, to,type);
       }
        
        bool StackHelper::begin_read_map(int indx) const {
            if (!lua_istable(m_L,m_base_index+indx)) return false;
            lua_pushnil(m_L);
            return true;
        }
        bool StackHelper::get_map_argument(int index) const {
            if (lua_next(m_L, m_base_index+index)==0) return false;
            if (!lua_isstring(m_L, -2)) {
                lua_pop(m_L, 2);
                return false;
            }
            return true;
        }
        void StackHelper::end_get_map_argument() const {
            lua_pop(m_L, 1); // pop value
        }
        void StackHelper::end_read_map() const {
            
        }
        
        void StackHelper::CallVoid(int numArgs) const {
            int res = lua_pcall(m_L, numArgs, 0, 0);
            if (res) {
                LogError(MODULE) << "Failed StackHelper::CallVoid : " << lua_tostring(m_L, -1);
            }
        }
	}
}
