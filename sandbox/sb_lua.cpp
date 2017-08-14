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
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

#include <ghl_types.h>
#include <ghl_data_stream.h>
#include <ghl_vfs.h>
#include <ghl_time.h>
#include <algorithm>

#include "sb_inplace_string.h"
#include "sb_log.h"
#include "sb_file_provider.h"
#include "sb_memory_mgr.h"
#include "sb_lua_context.h"
#include "sb_data.h"

#if !defined(SB_DNT_USE_MEM_MGR) && !defined(GHL_PLATFORM_FLASH) && !defined(GHL_PLATFORM_EMSCRIPTEN)
#define SB_USE_MEM_MGR
#endif

SB_META_BEGIN_KLASS_BIND(Sandbox::BinaryData)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_NAMED_KLASS(Sandbox::Logger, "log",void)

namespace Sandbox {
	
    static const char* MODULE = "Sanbox:Lua";
    
	static lua_State* g_terminate_context = 0;
    
    lua_State* g_terminate_thread = 0;
    
	bool lua_terminate_handler() {
        if (g_terminate_context) {
            LogError(MODULE) << "--- terminate ---";
            const char* text = "unknown";
            if (lua_gettop(g_terminate_context)<1 || !lua_isstring(g_terminate_context, -1)){
                //text = "unknown";
            } else {
                text = lua_tostring(g_terminate_context, -1);
            }
            luaL_traceback(g_terminate_context,
                           g_terminate_thread?g_terminate_thread:g_terminate_context,text,1);
            LogError(MODULE) << lua_tostring(g_terminate_context, -1);
            lua_pop(g_terminate_context, 1);
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
	
	
    
    template <GHL::LogLevel level>
    static int lua_log_func(lua_State* L) {
        int n = lua_gettop(L);  /* number of arguments */
        int i;
        lua_getglobal(L, "tostring");
        Logger log_print(level,"lua");
        for (i=1; i<=n; i++) {
            const char *s;
            lua_pushvalue(L, -1);  /* function to be called */
            lua_pushvalue(L, i);   /* value to print */
            lua_call(L, 1, 1);
            s = lua_tostring(L, -1);  /* get result */
            if (s == NULL)
                return luaL_error(L, LUA_QL("tostring") " must return a string to "
                                  LUA_QL("print"));
            if (i>1) log_print << "\t";
            log_print << s;
            lua_pop(L, 1);  /* pop result */
        }
        lua_pop(L, 1); // func
        return 0;
    }
    
    namespace meta {
        template <> template <class U> void bind_type<Logger>::bind(U& bind) {
            typedef Logger ThisType;(void)bind;
            bind(static_method("debug", &lua_log_func<GHL::LOG_LEVEL_DEBUG>));
            bind(static_method("verbose", &lua_log_func<GHL::LOG_LEVEL_VERBOSE>));
            bind(static_method("info", &lua_log_func<GHL::LOG_LEVEL_INFO>));
            bind(static_method("warning", &lua_log_func<GHL::LOG_LEVEL_WARNING>));
            bind(static_method("error", &lua_log_func<GHL::LOG_LEVEL_ERROR>));
            bind(static_method("fatal", &lua_log_func<GHL::LOG_LEVEL_FATAL>));
            SB_META_STATIC_METHOD(SetPlatformLogEnabled)
        }
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
	
    static int lua_os_time_func(lua_State* L) {
        lua_pushnumber(L, GHL_GetTime(0));
        return 1;
    }
    static int lua_os_timems_func(lua_State* L) {
        GHL::TimeValue tv;
        GHL_GetTime(&tv);
        lua_pushnumber(L,tv.secs*1000+lua_Number(tv.usecs)/1000 );
        return 1;
    }
    
	
	int lua_os_open_func (lua_State *L) {
        static const luaL_Reg os_funcs_impl[] = {
			{"time", lua_os_time_func},
            {"timems", lua_os_timems_func},
			{NULL, NULL}
		};
        luaL_newlib(L, os_funcs_impl);
        return 1;
    }
    
    
    static int lua_typename_func(lua_State* L) {
        int t = lua_type(L, -1);
        if (t != LUA_TUSERDATA) {
            lua_pushstring(L, lua_typename(L, t));
        } else {
            luabind::data_holder* holder = reinterpret_cast<luabind::data_holder*>(lua_touserdata(L, -1));
            if (holder && holder->info) {
                lua_pushstring(L, holder->info->name);
            } else {
                lua_pushstring(L, lua_typename(L, LUA_TUSERDATA));
            }
        }
        return 1;
    }

    ////////////////////////////////////////////////////////////
    void lua_io_register( lua_State* L, FileProvider* provider );
    
    LuaVM::LuaVM(FileProvider *resources ) :
        m_resources( resources ), 
        m_L(0),
        m_mem_mgr(0)
    {
#ifdef SB_USE_MEM_MGR
        m_mem_mgr = new MemoryMgr();
#endif
        Create();
    }
    
    void LuaVM::Create() {
        sb_assert(!m_L);
        m_L = lua_newstate(&LuaVM::lua_alloc_func,this);
        
        static const luaL_Reg loadedlibs[] = {
            {"_G", luaopen_base},
            {LUA_LOADLIBNAME, luaopen_package},
            {LUA_COLIBNAME, luaopen_coroutine},
            {LUA_TABLIBNAME, luaopen_table},
            //{LUA_IOLIBNAME, luaopen_io},
            {LUA_OSLIBNAME, lua_os_open_func},
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
            {"print",   lua_log_func<GHL::LOG_LEVEL_INFO>},
            //{"require",lua_require_func},
            {"loadfile",lua_loadfile_func},
            {"typename",lua_typename_func},
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
        
        luabind::ExternClass<Logger>(m_L);
        
        lua_atpanic(m_L, &at_panic_func);
        
        lua_io_register(m_L,m_resources);
        
        g_terminate_context = m_L;
#ifdef SB_DEBUG
        sb_terminate_handler = &lua_terminate_handler;
#endif
        luabind::Initialize(m_L);
        luabind::ExternClass<BinaryData>(m_L);
        
        luabind::SetErrorHandler(m_L, luabind::lua_traceback);
    }
    
    LuaVM::~LuaVM() {
        Destroy();
#ifdef SB_USE_MEM_MGR
        delete m_mem_mgr;
#endif
    }
    
    void LuaVM::Destroy() {
        if (m_L) {
            luabind::Deinitialize(m_L);
            if (g_terminate_context == m_L) {
                g_terminate_context = 0;
#ifdef SB_DEBUG
                sb_terminate_handler = 0;
#endif
            }
            lua_close(m_L);
            m_L = 0;
        }
    }

       
    void LuaVM::SetErrorHandler(LuaCFunction func) {
        if (m_L) {
            luabind::SetErrorHandler(m_L, func);
        }
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
        luabind::PushErrorHandler(m_L);
        int traceback_index = lua_gettop(m_L);
        sb::string mname = sb::string("@") + name;
        int res = lua_load(m_L,&lua_read_func,&data,mname.c_str(),0);  /// MF tf

        if (res!=0) {
            LogError(MODULE) << "Failed to load script: " << name;
            LogError(MODULE) << lua_tostring(m_L, -1) ;
			return false;
		} else {
            LogVerbose(MODULE) << "Loaded script: " << name;
            
            if (env) {
                env->GetObject(m_L);
                lua_setupvalue(m_L, -2, 1);  /* set it as 1st upvalue of loaded chunk */
            }
            //
            int res = lua_pcall(m_L, 0, results, -2);
            if (res) {
                LogError(MODULE) << "pcall : " << res;
                const char* err = lua_tostring(m_L, -1) ;
                LogError(MODULE) << (err ? err : "failed");
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


    void LuaVM::DoGC() {
        if (m_L)
            lua_gc(m_L, LUA_GCCOLLECT, 0);
    }

    
   
    int LuaVM::lua_module_searcher(lua_State *L) {
        LuaVM* this_ = reinterpret_cast<LuaVM*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char *name = luaL_checkstring(L, 1);
        
        sb::string path = this_->m_base_path;
        const char *begin = name;
        if (*begin!='.' && *begin!='/') {
            for (const char* s = name; *s; ++s) {
                if (*s=='.') {
                    if (s[1]=='.') {
                        ++s;
                    } else {
                        path.append(begin, s);
                        path.append("/");
                        begin = s+1;
                    }
                }
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
        sb::string mname = sb::string("@") + name;
		int res = lua_load(L,&lua_read_func,&data,mname.c_str(),0);
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
		else {
            GHL::Byte* new_data = _this->realloc(ptr,osize,nsize);
			return new_data;
		}
		return ptr;
	}
    
    LuaContextPtr  LuaVM::CreateContext() {
        lua_newtable(m_L);
        lua_pushglobaltable(m_L);
        lua_setfield(m_L, -2, "_G");
        LuaContextPtr ctx(new LuaContext());
        ctx->SetObject(m_L);
        return ctx;
    }
    
    LuaContextPtr   LuaVM::CreateObject() {
        lua_newtable(m_L);
        LuaContextPtr ctx(new LuaContext());
        ctx->SetObject(m_L);
        return ctx;
    }
    
    LuaContextPtr   LuaVM::GetGlobalContext() {
        lua_checkstack(m_L, 4);
        lua_pushglobaltable(m_L);
        LuaContextPtr ctx(new LuaContext());
        sb_assert(lua_istable(m_L, -1));
        ctx->SetObject(m_L);
        return ctx;
    }
    
    sb::string LuaVM::GetMemoryUsed() const {
#ifdef SB_USE_MEM_MGR
        return sb::string("lua:") + format_memory(lua_gc(m_L,LUA_GCCOUNT,0)*1024) +
                "/"+format_memory(m_mem_mgr->allocated());
#else
        return sb::string("lua:") + format_memory(lua_gc(m_L,LUA_GCCOUNT,0)*1024);
#endif
    }
    GHL::Byte* LuaVM::alloc(size_t size) {
#ifdef SB_USE_MEM_MGR
		return m_mem_mgr->alloc(size);
#else
        return (GHL::Byte*)::malloc(size);
#endif
	}
	void LuaVM::free(GHL::Byte* data,size_t size) {
#ifdef SB_USE_MEM_MGR
		m_mem_mgr->free(data,size);
#else
        ::free(data);
#endif
	}
	
    
    GHL::Byte* LuaVM::realloc(GHL::Byte* ptr,size_t osize,size_t nsize) {
#ifdef SB_USE_MEM_MGR
        GHL::Byte* res = m_mem_mgr->realloc(ptr,osize, nsize);
        return res;
#else
        return (GHL::Byte*)::realloc(ptr,nsize);
#endif
    }
}
