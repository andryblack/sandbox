/*
 *  sb_lua.h
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_LUA_H
#define SB_LUA_H


#include <sbstd/sb_shared_ptr.h>
#include <sbstd/sb_intrusive_ptr.h>
#include "sb_notcopyable.h"
#include <sbstd/sb_string.h>

#include <ghl_types.h>

struct lua_State;

namespace GHL {
    struct DataStream;
}

namespace Sandbox {
    
    class FileProvider;
    
    class MemoryMgr;
    
    class LuaContext;
    typedef sb::intrusive_ptr<LuaContext> LuaContextPtr;
	
    extern lua_State* g_terminate_thread;
    
    class LuaVM : public NotCopyable {
    public:
        explicit LuaVM( FileProvider* resources );
        ~LuaVM();
        void SetBasePath(const char* path);
        bool DoFile(const char* fn);
        sb::string GetMemoryUsed() const;
        
        lua_State* GetVM() { return m_L; }
        
        LuaContextPtr   GetGlobalContext();
        LuaContextPtr   CreateContext();

        bool   LoadScript(GHL::DataStream* ds,const char* name,const LuaContextPtr& env);
        
        void DoGC();
    private:
        FileProvider*  m_resources;
        lua_State*  m_L;
        GHL::UInt32 m_mem_use;
        sb::string m_base_path;
		GHL::Byte* alloc(size_t size);
        static int lua_module_searcher(lua_State *L);
        static int lua_loadfile_func(lua_State* L);
        static int lua_dofile_func(lua_State* L);
		void free(GHL::Byte* data,size_t size);
		void resize(GHL::Byte* data,size_t osize,size_t nsize);
        GHL::Byte* realloc(GHL::Byte* data,size_t osize,size_t nsize);
        static void* lua_alloc_func (void *ud, void *_ptr, size_t osize,size_t nsize);
        MemoryMgr*  m_mem_mgr;
        bool DoFileImpl(const char* name,int results);
        bool DoFileImpl(GHL::DataStream* ds,const char* name,int results,const LuaContextPtr& env);
    };
}

#endif /*SB_LUA_H*/
