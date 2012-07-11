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
    
    namespace luabind {
        class LuaRegistrator;
    }
	
    class Resources;
    
	class LuaVM;
	struct LuaVMHelper {
		LuaVM* lua;
	};
	typedef sb::shared_ptr<LuaVMHelper> LuaVMHelperPtr;
	typedef sb::weak_ptr<LuaVMHelper> LuaVMHelperWeakPtr;
	class LuaReference {
	public:
		explicit LuaReference( const LuaVMHelperWeakPtr& ptr );
		~LuaReference();
		void SetObject( lua_State* state );
		void UnsetObject( lua_State* state );
		void GetObject( lua_State* state );
		LuaVMHelperPtr GetHelper() const { return m_lua.lock();}
		const LuaVMHelperWeakPtr& GetHelperPtr() const { return m_lua;}
		bool Valid() const;
	private:
		LuaVMHelperWeakPtr m_lua;
		int	m_ref;
	};
		
	
    class LuaVM : public NotCopyable {
    public:
        explicit LuaVM( Resources* resources );
        ~LuaVM();
        void SetBasePath(const char* path);
        bool DoFile(const char* fn);
        GHL::UInt32 GetMemoryUsed() const { return m_mem_use;}
        
        lua_State* GetVM() { return m_L; }
        luabind::LuaRegistrator& GetRegistrator();
        
        const LuaVMHelperPtr& GetHelper() const { return m_helper; }
        static LuaVM* GetInstance( lua_State* L ); 
    private:
        Resources*  m_resources;
        lua_State*  m_L;
        LuaVMHelperPtr m_helper;
        GHL::UInt32 m_mem_use;
        sb::string m_base_path;
		GHL::Byte* alloc(size_t size);
        static int lua_module_searcher(lua_State *L);
        static int lua_loadfile_func(lua_State* L);
		void free(GHL::Byte* data,size_t size);
		void resize(GHL::Byte* data,size_t osize,size_t nsize);
        static void* lua_alloc_func (void *ud, void *_ptr, size_t osize,size_t nsize);
        luabind::LuaRegistrator*    m_registrator;
    };
}

#endif /*SB_LUA_H*/
