/*
 *  sb_lua.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_lua.h"
#include "luabind/sb_luabind.h"

extern "C" {
#include "../lua/src/lua.h"
#include "../lua/src/lauxlib.h"
#include "../lua/src/lualib.h"
}

#include <ghl_types.h>
#include <ghl_data_stream.h>
#include <ghl_vfs.h>
#include <algorithm>

#include "sb_inplace_string.h"
#include "sb_event.h"
#include "sb_log.h"
#include "sb_resources.h"

namespace Sandbox {
	
    static const char* MODULE = "Sanbox:Lua";
    
	
	LuaReference::LuaReference( const LuaVMHelperWeakPtr& ptr ) : m_lua(ptr),m_ref(LUA_NOREF) {
	}
	LuaReference::~LuaReference() {
		if (LuaVMHelperPtr lua = m_lua.lock()) {
			UnsetObject(lua->lua->GetVM());
		}
	}
	bool LuaReference::Valid() const {
		return m_ref!=LUA_NOREF;
	}
	void LuaReference::SetObject( lua_State* state ) {
		sb_assert(m_ref==LUA_NOREF);
		m_ref = luaL_ref(state,LUA_REGISTRYINDEX);
		sb_assert(m_ref!=LUA_NOREF);
	}
	void LuaReference::UnsetObject( lua_State* state ) {
		sb_assert(m_ref!=LUA_NOREF);
		luaL_unref(state,LUA_REGISTRYINDEX,m_ref);
		m_ref = LUA_NOREF;
	}
	void LuaReference::GetObject( lua_State* state ) {
		sb_assert(m_ref!=LUA_NOREF);
        lua_rawgeti(state, LUA_REGISTRYINDEX, m_ref);
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
	
	static int lua_print_func (lua_State *L) {
        int n = lua_gettop(L);  /* number of arguments */
        int i;
        lua_getglobal(L, "tostring");
        LogInfo log_info("Lua:Scripts");
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
        lua_pop(L, 1); // func
      	  return 0;
    }
		
	
	
	// traceback function, adapted from lua.c
	// when a runtime error occurs, this will append the call stack to the error message
	
    
    static int at_panic_func(lua_State* L) {
        LogError(MODULE) << "--- panic ---";
        if (!lua_isstring(L, -1)){
            lua_pushstring(L, "unknown");
        }
        lua_pushcclosure(L, &luabind::lua_traceback, 0);
        lua_pcall(L, 1, 1, 0);
        LogError(MODULE) << "--- panic ---";
        LogError(MODULE) << lua_tostring(L, -1);
        exit(1);
        return 0;
    }
	
	
	
    ////////////////////////////////////////////////////////////
    
    
    LuaVM::LuaVM( Resources* resources ) : 
        m_resources( resources ), 
        m_L(0),
        m_mem_use(0)
    {
        m_L = lua_newstate(&LuaVM::lua_alloc_func,this);
        m_helper = LuaVMHelperPtr( new LuaVMHelper() );
        m_helper->lua = this;
        
        static const luaL_Reg loadedlibs[] = {
            {"_G", luaopen_base},
            {LUA_LOADLIBNAME, luaopen_package},
            {LUA_COLIBNAME, luaopen_coroutine},
            {LUA_TABLIBNAME, luaopen_table},
            //{LUA_IOLIBNAME, luaopen_io},
            //{LUA_OSLIBNAME, luaopen_os},
            {LUA_STRLIBNAME, luaopen_string},
            {LUA_BITLIBNAME, luaopen_bit32},
            {LUA_MATHLIBNAME, luaopen_math},
            {LUA_DBLIBNAME, luaopen_debug},
            {NULL, NULL}
        };

		const luaL_Reg *lib;
        /* call open functions from 'loadedlibs' and set results to global table */
        for (lib = loadedlibs; lib->func; lib++) {
            luaL_requiref(m_L, lib->name, lib->func, 1);
            lua_pop(m_L, 1);  /* remove lib */
        }
        
        static const luaL_Reg base_funcs_impl[] = {
			//{"dofile", lua_dofile_func},
			{"print", lua_print_func},
            //{"require",lua_require_func},
            {"loadfile",lua_loadfile_func},
			{NULL, NULL}
		};
        lua_rawgeti(m_L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
        lua_pushlightuserdata(m_L, this);
        luaL_setfuncs(m_L,  base_funcs_impl,1);
        lua_pop(m_L, 1);

        lua_createtable(m_L, 1, 0);
        lua_pushlightuserdata(m_L, this);
        lua_pushcclosure(m_L, &LuaVM::lua_module_searcher, 1);
        lua_rawseti(m_L,-2,1);
        luabind::lua_set_value(m_L, "package.searchers");
        
        lua_pushlightuserdata(m_L,this);
		lua_setglobal(m_L, "LuaVM_instance");
        
		lua_atpanic(m_L, &at_panic_func);
    }
    
    LuaVM::~LuaVM() {
        if (m_L) {
            lua_pushnil(m_L);
            lua_setglobal(m_L, "LuaVM_instance");
            m_helper = LuaVMHelperPtr();
            lua_close(m_L);
            m_L = 0;
        }
    }
    
    LuaVM* LuaVM::GetInstance( lua_State* L ) {
        lua_getglobal(L, "LuaVM_instance");
        LuaVM* this_ = lua_isuserdata(L, -1) ? reinterpret_cast<LuaVM*>(lua_touserdata(L, -1)) : 0;
        lua_pop(L, 1);
        return this_;
    }
    
    void LuaVM::SetBasePath(const char *path) {
        m_base_path = path;
        if (!m_base_path.empty() && m_base_path[m_base_path.length()-1]!='/')
            m_base_path += "/";
    }
    
    bool LuaVM::DoFile(const char* fn) {
        sb::string path = m_base_path + fn;
        GHL::DataStream* ds = m_resources->OpenFile(path.c_str());
        if (!ds) {
			LogError(MODULE) << "error opening file " << fn;
			return false;
		}
		lua_read_data data = {ds,{}};
		int res = lua_load(m_L,&lua_read_func,&data,fn,0);
		ds->Release();
		if (res!=0) {
			LogError(MODULE) << "Failed to load script: " << fn;
            LogError(MODULE) << lua_tostring(m_L, -1) ;
			return false;
		} else {
			LogInfo(MODULE) << "Loaded script: " << fn;
            
            lua_pushcclosure(m_L, &luabind::lua_traceback, 0);
            lua_insert(m_L, -2);
            int res = lua_pcall(m_L, 0, 0, -2);
            if (res) {
                LogError(MODULE) << "pcall : " << res;
                LogError(MODULE) << lua_tostring(m_L, -1) ;
                return false;
            }
            lua_remove(m_L, -1);
		}
        
		return true;
    }
    
   
    int LuaVM::lua_module_searcher(lua_State *L) {
        LuaVM* this_ = reinterpret_cast<LuaVM*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char *name = luaL_checkstring(L, 1);
        sb::string path = this_->m_base_path + name + ".lua";
        GHL::DataStream* ds = this_->m_resources->OpenFile(path.c_str());
        if (!ds) {
			LogError(MODULE) << "error opening module file " << name;
			return 0;
		}
		lua_read_data data = {ds,{}};
		int res = lua_load(L,&lua_read_func,&data,name,0);
		ds->Release();
		if (res!=0) {
			LogError(MODULE) << "Failed to load module: " << name;
            LogError(MODULE) << lua_tostring(L, -1) ;
			return 0;
		} 
        lua_pushstring(L, name);
        return 2;
    }
    int LuaVM::lua_loadfile_func(lua_State* L) {
        const char *fname = luaL_optstring(L, 1, NULL);
        //const char *mode = luaL_optstring(L, 2, NULL);
        int env = !lua_isnone(L, 3);  /* 'env' parameter? */
        if (!fname) {
            lua_pushnil(L);
            lua_pushstring(L, "file name not specified");
            return 2;
        }
        LuaVM* this_ = reinterpret_cast<LuaVM*>(lua_touserdata(L, lua_upvalueindex(1)));
        GHL::DataStream* ds = this_->m_resources->OpenFile(fname);
        if (!ds) {
            lua_pushnil(L);
            lua_pushstring(L, "error opening file");
            return 2;
		}
		lua_read_data data = {ds,{}};
		int res = lua_load(L,&lua_read_func,&data,fname,0);
		ds->Release();
		if (res!=0) {
            lua_pushnil(L);
            lua_pushstring(L, "error loading file");
            return 2;
		} 
        if ( env ) {  /* 'env' parameter? */
            lua_pushvalue(L, 3);
            lua_setupvalue(L, -2, 1);  /* set it as 1st upvalue of loaded chunk */
        }
        return 1;
    }
    void* LuaVM::lua_alloc_func(void *ud, void *_ptr, size_t osize,size_t nsize) {
		GHL::Byte* ptr = reinterpret_cast<GHL::Byte*> (_ptr);
		LuaVM* _this = static_cast<LuaVM*>(ud);    
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
    
    GHL::Byte* LuaVM::alloc(size_t size) {
		m_mem_use+=GHL::UInt32(size);
		return new GHL::Byte[size];
	}
	void LuaVM::free(GHL::Byte* data,size_t size) {
		sb_assert(m_mem_use>=size);
		m_mem_use-=GHL::UInt32(size);
		delete [] (data);
	}
	void LuaVM::resize(GHL::Byte*,size_t osize,size_t nsize) {
		sb_assert(m_mem_use>=osize);
		m_mem_use-=GHL::UInt32(osize);
		m_mem_use+=GHL::UInt32(nsize);
	}
    
}
