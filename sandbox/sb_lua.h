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


#include "sb_shared_ptr.h"
#include "sb_notcopyable.h"
#include "sb_string.h"

#include <vector>

#include <ghl_types.h>

struct lua_State;

namespace GHL {
	struct VFS;
}

namespace Sandbox {
    
    class Resources;
    
	class LuaVM;
	
		
	
    class LuaVM : public NotCopyable {
    public:
        explicit LuaVM( Resources* resources );
        ~LuaVM();
        void SetBasePath(const char* path);
        bool DoFile(const char* fn);
        GHL::UInt32 GetMemoryUsed() const { return m_mem_use;}
        
        lua_State* GetVM() { return m_L; }
        
        static LuaVM* GetInstance( lua_State* L ); 
    private:
        Resources*  m_resources;
        lua_State*  m_L;
        GHL::UInt32 m_mem_use;
        sb::string m_base_path;
		GHL::Byte* alloc(size_t size);
        static int lua_module_searcher(lua_State *L);
        static int lua_loadfile_func(lua_State* L);
		void free(GHL::Byte* data,size_t size);
		void resize(GHL::Byte* data,size_t osize,size_t nsize);
        static void* lua_alloc_func (void *ud, void *_ptr, size_t osize,size_t nsize);
    };
}

#endif /*SB_LUA_H*/
