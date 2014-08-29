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
#include <sbstd/sb_assert.h>

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
#include "sb_file_provider.h"
#include "sb_memory_mgr.h"
#include "sb_lua_context.h"

namespace Sandbox {
	
    static const char* MODULE = "Sanbox:Lua";
    
	static lua_State* g_terminate_context = 0;
	bool lua_terminate_handler() {
        if (g_terminate_context) {
            LogError(MODULE) << "--- terminate ---";
            lua_pushcclosure(g_terminate_context, &luabind::lua_traceback, 0);
            if (lua_gettop(g_terminate_context)<2 || !lua_isstring(g_terminate_context, -2)){
                lua_pushstring(g_terminate_context, "unknown");
            } else {
                lua_pushvalue(g_terminate_context, -2);
            }
            lua_pcall(g_terminate_context, 1, 1, 0);
            LogError(MODULE) << lua_tostring(g_terminate_context, -1);
            LogError(MODULE) << "--- terminate ---";
        }
        return false;
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
        lua_pushcclosure(L, &luabind::lua_traceback, 0);
        if (!lua_isstring(L, -2)){
            lua_pushstring(L, "unknown");
        } else {
            lua_pushvalue(L, -2);
        }
        lua_pcall(L, 1, 1, 0);
        LogError(MODULE) << "--- panic ---";
        LogError(MODULE) << lua_tostring(L, -1);
#ifdef SB_DEBUG
        sb_terminate_handler = 0;
        sb_terminate();
#endif
        return 0;
    }
	
	
	
    ////////////////////////////////////////////////////////////
    
    
    LuaVM::LuaVM(FileProvider *resources ) :
        m_resources( resources ), 
        m_L(0),
        m_mem_use(0)
    {
        m_mem_mgr = new MemoryMgr();
        m_L = lua_newstate(&LuaVM::lua_alloc_func,this);
        
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
			{"dofile", lua_dofile_func},
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
        
		lua_atpanic(m_L, &at_panic_func);
        
        g_terminate_context = m_L;
#ifdef SB_DEBUG
        sb_terminate_handler = &lua_terminate_handler;
#endif
        luabind::Initialize(m_L);
    }
    
    LuaVM::~LuaVM() {
        if (m_L) {
            luabind::Deinitialize(m_L);
            g_terminate_context = 0;
#ifdef SB_DEBUG
            sb_terminate_handler = 0;
#endif
            lua_close(m_L);
            m_L = 0;
        }
        if (m_mem_use) {
            LogWarning() << "Lua unfree " << format_memory(m_mem_use);
        }
        delete m_mem_mgr;
    }
    

    
    void LuaVM::SetBasePath(const char *path) {
        m_base_path = path;
        if (!m_base_path.empty() && m_base_path[m_base_path.length()-1]!='/')
            m_base_path += "/";
    }
    
    bool LuaVM::DoFileImpl(const char* fn,int results) {
        sb::string path = m_base_path + fn;
        GHL::DataStream* ds = m_resources->OpenFile(path.c_str());
        if (!ds) {
			LogError(MODULE) << "error opening file " << fn;
			return false;
		}
        bool res = DoFileImpl(ds,fn,results,LuaContextPtr());
        ds->Release();
        return res;
    }
    bool LuaVM::DoFileImpl(GHL::DataStream* ds, const char* name, int results, const LuaContextPtr &env) {
		lua_read_data data = {ds,{}};
        lua_pushcclosure(m_L, &luabind::lua_traceback, 0);  /// tb
        int traceback_index = lua_gettop(m_L);
        
        int res = lua_load(m_L,&lua_read_func,&data,name,0);  /// MF tf

        if (res!=0) {
            LogError(MODULE) << "Failed to load script: " << name;
            LogError(MODULE) << lua_tostring(m_L, -1) ;
			return false;
		} else {
            LogInfo(MODULE) << "Loaded script: " << name;
            
            if (env) {
                env->GetObject(m_L);
                lua_setupvalue(m_L, -2, 1);  /* set it as 1st upvalue of loaded chunk */
            }
            //
            int res = lua_pcall(m_L, 0, results, -2);
            if (res) {
                LogError(MODULE) << "pcall : " << res;
                LogError(MODULE) << lua_tostring(m_L, -1) ;
                return false;
            }
            lua_remove(m_L, traceback_index);
            //int crnt_top = lua_gettop(m_L);
            //lua_remove(m_L, crnt_top-traceback_index-1);
		}
        
		return true;
    }

    bool LuaVM::DoFile(const char* fn) {
        return DoFileImpl(fn,0);
    }

    bool   LuaVM::LoadScript(GHL::DataStream* ds, const char *name, const LuaContextPtr& env) {
        return DoFileImpl(ds,name,0,env);
    }



//    
//    bool LuaVM::DoString( const char* cont ) {
//        //LogInfo(MODULE) << "pcall >>>> top : " << lua_gettop(m_L);
//        lua_pushcclosure(m_L, &luabind::lua_traceback, 0);  /// tb
//        int traceback_index = lua_gettop(m_L);
//        int res = luaL_loadstring(m_L, cont);
//        if (res!=0) {
//			LogError(MODULE) << "Failed to load script: " << cont;
//            LogError(MODULE) << lua_tostring(m_L, -1) ;
//			return false;
//		} else {
//			  //
//            res = lua_pcall(m_L, 0, 0, -2);
//            if (res) {
//                LogError(MODULE) << "pcall : " << res;
//                LogError(MODULE) << lua_tostring(m_L, -1) ;
//                return false;
//            }
//            lua_remove(m_L, traceback_index);
//     	}
//        //LogInfo(MODULE) << "pcall <<<< top : " << lua_gettop(m_L);
//        return true;
//    }
    
   
    int LuaVM::lua_module_searcher(lua_State *L) {
        LuaVM* this_ = reinterpret_cast<LuaVM*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char *name = luaL_checkstring(L, 1);
        
        sb::string path = this_->m_base_path;
        const char *begin = name;
        for (const char* s = name; *s; ++s) {
            if (*s=='.') {
                path.append(begin, s);
                path.append("/");
                begin = s+1;
            }
        }
        path.append(begin);
        path.append(".lua");
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
        sb::string filename =  fname;
        GHL::DataStream* ds = this_->m_resources->OpenFile(filename.c_str());
        if (!ds) {
            lua_pushnil(L);
            lua_pushstring(L, (sb::string("error opening file: ")+filename).c_str());
            return 2;
		}
		lua_read_data data = {ds,{}};
		int res = lua_load(L,&lua_read_func,&data,fname,0);
		ds->Release();
		if (res!=0) {
            lua_pushstring(L, (sb::string("error loading file: ")+filename).c_str());
            return 2;
		} 
        if ( env ) {  /* 'env' parameter? */
            lua_pushvalue(L, 3);
            lua_setupvalue(L, -2, 1);  /* set it as 1st upvalue of loaded chunk */
        }
        return 1;
    }
        
    int LuaVM::lua_dofile_func(lua_State* L) {
        LuaVM* this_ = reinterpret_cast<LuaVM*>(lua_touserdata(L, lua_upvalueindex(1)));
        int cnt = lua_gettop(L);
        if (this_->DoFileImpl(lua_tostring(L, 1),LUA_MULTRET)) {
            int results = lua_gettop(L)-cnt;
            return results;
        }
        lua_pushstring(L, "error dofile");
        lua_error(L);
        return 0;
    }
    void* LuaVM::lua_alloc_func(void *ud, void *_ptr, size_t osize,size_t nsize) {
		GHL::Byte* ptr = reinterpret_cast<GHL::Byte*> (_ptr);
		LuaVM* _this = static_cast<LuaVM*>(ud);    
        if (!ptr) {
            if (nsize)
                return _this->alloc(nsize);
        }else if (nsize == 0) {
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
    
    LuaContextPtr  LuaVM::CreateContext() {
        lua_newtable(m_L);
        lua_getglobal(m_L, "_G");
        lua_setfield(m_L, -2, "_G");
        LuaContextPtr ctx(new LuaContext());
        ctx->SetObject(m_L);
        return ctx;
    }
    
    LuaContextPtr   LuaVM::GetGlobalContext() {
        lua_getglobal(m_L, "_G");
        LuaContextPtr ctx(new LuaContext());
        ctx->SetObject(m_L);
        return ctx;
    }
    
    sb::string LuaVM::GetMemoryUsed() const {
        sb::string res = sb::string("lua:") + format_memory(m_mem_use) +
        "/"+format_memory(m_mem_mgr->allocated());
        return res;
    }
    GHL::Byte* LuaVM::alloc(size_t size) {
		m_mem_use+=GHL::UInt32(size);
		return m_mem_mgr->alloc(size);
	}
	void LuaVM::free(GHL::Byte* data,size_t size) {
		sb_assert(m_mem_use>=size);
		m_mem_use-=GHL::UInt32(size);
		m_mem_mgr->free(data);
	}
	void LuaVM::resize(GHL::Byte*,size_t osize,size_t nsize) {
		sb_assert(m_mem_use>=osize);
		m_mem_use-=GHL::UInt32(osize);
		m_mem_use+=GHL::UInt32(nsize);
	}
    
}
