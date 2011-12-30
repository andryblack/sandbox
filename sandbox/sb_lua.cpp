/*
 *  sb_lua.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_lua.h"

extern "C" {
#include "../lua/src/lua.h"
#include "../lua/src/lauxlib.h"
	LUALIB_API int (luaopen_base) (lua_State *L);
	LUALIB_API int (luaopen_table) (lua_State *L);
	LUALIB_API int (luaopen_string) (lua_State *L);
	LUALIB_API int (luaopen_math) (lua_State *L);
	LUALIB_API int (luaopen_debug) (lua_State *L);
}

#include <ghl_types.h>
#include <ghl_data_stream.h>
#include <ghl_vfs.h>
#include <algorithm>

#include "sb_inplace_string.h"
#include "sb_bind.h"
#include "sb_event.h"
#include "sb_log.h"

namespace Sandbox {
	
    static const char* MODULE = "Sanbox:Lua";
    
	
	LuaReference::LuaReference( const LuaHelperWeakPtr& ptr ) : m_lua(ptr),m_ref(LUA_NOREF) {
	}
	LuaReference::~LuaReference() {
		if (LuaHelperPtr lua = m_lua.lock()) {
			UnsetObject(lua->lua->GetVM());
		}
	}
	bool LuaReference::Valid() const {
		return m_ref!=LUA_NOREF;
	}
	void LuaReference::SetObject( lua_State* state ) {
		sb_assert(m_ref==LUA_NOREF);
		m_ref = lua_ref(state,true);
		sb_assert(m_ref!=LUA_NOREF);
	}
	void LuaReference::UnsetObject( lua_State* state ) {
		sb_assert(m_ref!=LUA_NOREF);
		lua_unref(state,m_ref);
		m_ref = LUA_NOREF;
	}
	void LuaReference::GetObject( lua_State* state ) {
		sb_assert(m_ref!=LUA_NOREF);
		lua_getref(state,m_ref);
	}
		
	
	LuaFunction::LuaFunction( const LuaHelperWeakPtr& ptr ) : m_ref(ptr) {
		if (LuaHelperPtr lua = m_ref.GetHelper()) {
			sb_assert( lua_isfunction(lua->lua->GetVM(),-1) );
			m_ref.SetObject(lua->lua->GetVM());
		}
	}
	LuaEnvironmentPtr LuaFunction::GetEnv() {
		if (m_ref.Valid()) {
			if (LuaHelperPtr lua = m_ref.GetHelper()) {
				lua_State* L = lua->lua->GetVM();
				m_ref.GetObject(L);
				sb_assert( lua_isfunction(L,-1) );
				lua_getfenv(L, -1);
				sb_assert( lua_istable(L,-1) );
				LuaEnvironmentPtr env = LuaEnvironmentPtr(new LuaEnvironment(m_ref.GetHelperPtr(),-1));
				lua_pop(L,1);
				return env;
			}
		}
		return LuaEnvironmentPtr();
	}
	static int lua_error_function( lua_State* L ) {
		LogError(MODULE) << "error exec inline function" ;
		LogError(MODULE) << lua_tostring(L, -1) ;
		/// remove error from stack
		lua_pop(L,1);
		return 0;
	}
	void LuaFunction::Call() {
		if (m_ref.Valid()) {
			if (LuaHelperPtr lua = m_ref.GetHelper()) {
				lua_pushcclosure(lua->lua->GetVM(), &lua_error_function, 0);
				m_ref.GetObject(lua->lua->GetVM());
				lua_pcall(lua->lua->GetVM(), 0, 0, -2);
			}
		}
	}
	
	LuaEnvironment::LuaEnvironment( const LuaHelperWeakPtr& ptr,int indx ) : m_ref(ptr) {
		if (LuaHelperPtr lua = m_ref.GetHelper()) {
			lua_State* L = lua->lua->GetVM();
			if (indx==0) {
				lua_newtable(L);											// [1] t
				lua_createtable(L, 0, 1); /* create new metatable */	// [2] mt t
				lua_pushvalue(L, -1);									// [3] mt mt t
				lua_setmetatable(L, -3);								// [2] mt t
				lua_pushvalue(L, LUA_GLOBALSINDEX);						// [3] _G mt t
				lua_setfield(L, -2, "__index");  /* mt.__index = _G */  // [2] mt t
				lua_pop(L,1);
			} else {
				sb_assert( lua_istable(L,-1) );
			}
			m_ref.SetObject(lua->lua->GetVM());
		}
	}
	
	void LuaEnvironment::GetEnv( lua_State* state ) {
		m_ref.GetObject(state);
	}
	
	LuaEnvironment::~LuaEnvironment() {
	}
	
	static const char * lua_string_read_func (lua_State* /*L*/,void *data,size_t *size) {
		const char** d = reinterpret_cast<const char**> (data);
		size_t need = *size;
		const char* p = *d;
		while (*p && need) {
			need--;
			p++;
		}
		*size = p-*d;
		const char* res = *d;
		*d = p;
		return res;
	}
	
	struct lua_read_data {
		GHL::DataStream* ds;
		char buffer[512];
	};
	static const char * lua_read_func (lua_State* /*L*/,void *data,size_t *size) {
		lua_read_data* d = reinterpret_cast<lua_read_data*> (data);
		GHL::UInt32 readed = d->ds->Read(reinterpret_cast<GHL::Byte*>(d->buffer),512);
		*size = readed;
		return d->buffer;
	}
	
	LuaFunctionPtr LuaEnvironment::LoadFunction( const char* content ) {
		if (m_ref.Valid()) {
			if (LuaHelperPtr lua = m_ref.GetHelper()) {
				lua_State* L = lua->lua->GetVM();
				if (lua_load(L, &lua_string_read_func,&content, "inline")!=0) {
					LogError(MODULE) << "Failed to load inline : '" << content << "'";
                    LogError(MODULE) << lua_tostring(L, -1) ;
					lua_pop(L,1);
				} else {
					m_ref.GetObject( L );
					lua_setfenv(L, -2);
					return LuaFunctionPtr(new LuaFunction(m_ref.GetHelperPtr()) );
				}
			}
		}
		return LuaFunctionPtr();
	}
	
	LuaFunctionPtr LuaEnvironment::LoadFunction( const char* name, GHL::DataStream* stream ) {
		if (m_ref.Valid()) {
			if (LuaHelperPtr lua = m_ref.GetHelper()) {
				lua_State* L = lua->lua->GetVM();
				lua_read_data data = {stream,{}};
				int res = lua_load(L,&lua_read_func,&data,name);
				if (res!=0) {
					LogError(MODULE)<<"Failed to load script: " << name ;
                    LogError(MODULE)<< lua_tostring(L, -1) ;
					lua_pop(L,1);
				} else {
					m_ref.GetObject( L );
					lua_setfenv(L, -2);
					return LuaFunctionPtr(new LuaFunction(m_ref.GetHelperPtr()) );
				}
			}
		}
		return LuaFunctionPtr();
	}
	
	static int lua_print_func (lua_State *L) {
        int n = lua_gettop(L);  /* number of arguments */
        int i;
        lua_getglobal(L, "tostring");
        LogInfo log_info(MODULE);
        for (i=1; i<=n; i++) {
		    const char *s;
		    lua_pushvalue(L, -1);  /* function to be called */
		    lua_pushvalue(L, i);   /* value to print */
		    lua_call(L, 1, 1);
		    s = lua_tostring(L, -1);  /* get result */
		    if (s == NULL)
		      return luaL_error(L, LUA_QL("tostring") " must return a string to "
		                           LUA_QL("print"));
			  if (i>1) log_info << "\t";
			  log_info << s;  
		    lua_pop(L, 1);  /* pop result */
		  }
      	  return 0;
		}
		
	
	void *Lua::lua_alloc_func (void *ud, void *_ptr, size_t osize,size_t nsize) {
		GHL::Byte* ptr = reinterpret_cast<GHL::Byte*> (_ptr);
		Lua* _this = static_cast<Lua*>(ud);    
		if (nsize == 0) {
			if (ptr) _this->free(ptr,osize);
			return NULL;
		}
		else if (nsize > osize) {
			GHL::Byte* new_data = _this->alloc(nsize);
			if (ptr && osize) {
				::memcpy(new_data,ptr,osize);
				_this->free(ptr,osize);
			}
			return new_data;
		} else {
			_this->resize(ptr,osize,nsize);
		}
		return ptr;
	}
	
	int Lua::lua_dofile_func (lua_State *L) {
		int n = lua_gettop(L);
		Lua* l = Lua::GetPtr(L);
		if (l) {
			if (!l->load_file(L,lua_tostring(L,1))) {
				lua_pushstring(L, "error loading file");
				lua_error(L);
			} else {
				lua_call(L, 0, LUA_MULTRET);
			}
		} else {
			lua_pushstring(L, "not fund state");
			lua_error(L);
		}
		return lua_gettop(L)-n;
	}
		
	
	// traceback function, adapted from lua.c
	// when a runtime error occurs, this will append the call stack to the error message
	static int traceback (lua_State *L)
	{
		// look up Lua's 'debug.traceback' function
		lua_getglobal(L, "debug");
		if (!lua_istable(L, -1))
		{
			lua_pop(L, 1);
			return 1;
		}
		lua_getfield(L, -1, "traceback");
		if (!lua_isfunction(L, -1))
		{
			lua_pop(L, 2);
			return 1;
		}
		lua_pushvalue(L, 1);  /* pass error message */
		lua_pushinteger(L, 2);  /* skip this function and traceback */
		lua_call(L, 2, 1);  /* call debug.traceback */
		LogError(MODULE)<<"trap";
		exit(1);
	}
	
	
	Lua::Lua(GHL::VFS* vfs) : m_vfs(vfs) {
	
		m_mem_use = 0;
		
		m_state = lua_newstate( &lua_alloc_func,this);
		m_helper = LuaHelperPtr(new LuaHelper());
		m_helper->lua = this;
		
		
		static const luaL_Reg lualibs[] = {
			{"", luaopen_base},
			{"table", luaopen_table},
			{"string", luaopen_string},
			{"math", luaopen_math},
			{"debug", luaopen_debug},
			{NULL, NULL}
		};
		
		for (const luaL_Reg *lib = lualibs; lib->func; lib++) {
			lua_pushcfunction(m_state, lib->func);
			lua_pushstring(m_state, lib->name);
			lua_call(m_state, 1, 0);
		}
		
		static const luaL_Reg base_funcs_impl[] = {
			{"dofile", lua_dofile_func},
			{"print", lua_print_func},
			{NULL, NULL}
		};
		luaL_register(m_state, "_G", base_funcs_impl);
		lua_pushlightuserdata(m_state,this);
		lua_setglobal(m_state, "g_luaState");
		
		
		lua_atpanic(m_state, &traceback);
		
		RegisterSandboxObjects();
	}
	
	Lua::~Lua() {
		LogDebug(MODULE) <<  "Close";
		m_helper = LuaHelperPtr();
		lua_close(m_state);
		m_helper = LuaHelperPtr();
		LogDebug(MODULE) << "after close memory in use : " << m_mem_use;
	}
	
	Lua* Lua::GetPtr(lua_State* L) {
		lua_getglobal(L,"g_luaState");
		if (lua_islightuserdata(L,-1)) {
			Lua* l = reinterpret_cast<Lua*> (lua_touserdata(L,-1));
			lua_pop(L,1);
			return l;
		}
		return 0;
	}
	
	LuaEnvironmentPtr Lua::CreateEnvironment() {
		return LuaEnvironmentPtr( new LuaEnvironment( m_helper ) );
	}

	void Lua::SetBasePath(const char* path) {
		m_base_path = path;
	}
	
	bool Lua::load_file(lua_State* L,const char* fn) {
		std::string filename = m_base_path + fn;
		GHL::DataStream* ds = m_vfs->OpenFile(filename.c_str());
		if (!ds) {
			LogWarning(MODULE) << "error opening file " << filename;
			return false;
		}
		lua_read_data data = {ds,{}};
		int res = lua_load(L,&lua_read_func,&data,fn);
		ds->Release();
		if (res!=0) {
			LogError(MODULE) << "Failed to load script: " << filename;
            LogError(MODULE) << lua_tostring(m_state, -1) ;
			return false;
		} else {
			LogInfo(MODULE) << "Loaded script: " << fn;
		}
		return true;
	}
	
	bool Lua::call(const char* str,int args) {
		int res = lua_pcall(m_state, args, 0, 0);
		if (res) {
			LogError(MODULE) << "Failed to script call : " << str;
            LogError(MODULE) << lua_tostring(m_state, -1) ;
			return false;
		}
		return true;
	}
	bool Lua::DoFile(const char* fn) {
		if (!load_file(GetVM(),fn)) return false;
		return call(fn,0);
	}
	
	static const char* get_table_from( lua_State* L ,const char* name ) {
		InplaceString str(name);
		InplaceString tbl(str.begin(),str.find(".:"));
		while (tbl!=str) {
			lua_getfield(L, -1, tbl.str().c_str());
			if (!lua_istable(L,-1)) {
				lua_pop(L,1);
				lua_newtable(L);
				lua_pushvalue(L, -1);
				lua_setfield(L, -3, tbl.str().c_str());
			}
			lua_remove(L, -2);
			str = InplaceString(tbl.end()+1,str.end());
			tbl = InplaceString(str.begin(),str.find(".:"));
		}
		return str.begin();
	}
	
	const char* Lua::get_table(const char* name) {
		lua_State* L = GetVM();
		lua_getglobal(L,"_G");
		sb_assert(lua_istable(L,-1));
		return get_table_from(L,name);
	}
	const char* Lua::get_table(const LuaEnvironmentPtr& env,const char* name) {
		lua_State* L = GetVM();
		env->GetEnv(L);
		sb_assert(lua_istable(L,-1));
		return get_table_from(L,name);
	}
	
	void Lua::Call(const char* func) {
		do_call(func,0);
	}
	
	void Lua::do_call(const char* func,int args) {
		lua_State* L = GetVM();
		const char* str = get_table(func);
		lua_getfield(L, -1, str);
		if (lua_isfunction(L,-1)) {
			if ( (str!=func) && str[-1]==':') {
				sb_assert(lua_istable(L,-2));
				sb_assert(lua_isfunction(L,-1));
				lua_pushvalue(L, -2); /// args tbl func tbl
				for (int i=0;i<args;i++)
					lua_pushvalue(L, (-4-i)-args+1+i);
				call(func,1+args);
			} else {
				for (int i=0;i<args;i++)
					lua_pushvalue(L, (-3-i)-args+1+i);
				call(func,args);
			}
			// remove table
			lua_pop(L,1+args);
		} else {
			/// remove table and field
			lua_pop(L,2);
			LogError(MODULE) <<  "not found function " << func;
		}
	}
	
	void Lua::register_object(const char* name) {
		lua_State* L = GetVM();
		const char* str = get_table(name);
		lua_pushvalue(L, -2);
		Bind::StackHelper::rawsetfield(L,-2,str);
		lua_pop(L,2);
	}
	
	void Lua::register_object(const LuaEnvironmentPtr& env, const char* name) {
		lua_State* L = GetVM();
		const char* str = get_table(env,name);
		lua_pushvalue(L, -2);
		Bind::StackHelper::rawsetfield(L,-2,str);
		lua_pop(L,2);
	}
	
	namespace Bind {
		void register_enum(lua_State* L,const EnumBind* info);
		void register_type(lua_State* L,const ClassBind* bind);
	}
	
	void Lua::Bind(const Bind::ClassBind* info) {
		register_type(GetVM(),info);
	}
	
	void Lua::Bind(const Bind::EnumBind* info) {
		register_enum(GetVM(),info);
	}
	
	GHL::Byte* Lua::alloc(size_t size) {
		m_mem_use+=GHL::UInt32(size);
		return new GHL::Byte[size];
	}
	void Lua::free(GHL::Byte* data,size_t size) {
		sb_assert(m_mem_use>=size);
		m_mem_use-=GHL::UInt32(size);
		delete [] (data);
	}
	void Lua::resize(GHL::Byte*,size_t osize,size_t nsize) {
		sb_assert(m_mem_use>=osize);
		m_mem_use-=GHL::UInt32(osize);
		m_mem_use+=GHL::UInt32(nsize);
	}
}
