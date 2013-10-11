//
//  sb_rocket_document.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/13/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_rocket_document.h"
#include "sb_rocket_context.h"
#include "sb_lua.h"
#include "sb_log.h"

extern "C" {
#include "../lua/src/lua.h"
#include "../lua/src/lauxlib.h"
#include "../lua/src/lualib.h"
}

#include "luabind/sb_luabind_stack.h"

SB_META_DECLARE_KLASS(Sandbox::RocketDocument, Rocket::Core::ElementDocument)

namespace Sandbox {
    
    static const char* MODULE = "RocketDocument";
    
    
    RocketDocument::RocketDocument(const Rocket::Core::String& tag,Rocket::Core::Context* context) : Rocket::Core::ElementDocument(tag,context){
        RocketContext* tctx = reinterpret_cast<RocketContext*>(context);
        m_context = tctx->GetLuaVM()->CreateContext();
        m_context->SetValue("document", this);
    }
    
    
    struct rocket_read_data {
		Rocket::Core::Stream* stream;
		char buffer[512];
	};
	static const char * rocket_read_func (lua_State* /*L*/,void *data,size_t *size) {
		rocket_read_data* d = reinterpret_cast<rocket_read_data*> (data);
		size_t readed = d->stream->Read(d->buffer,512);
		*size = readed;
		return d->buffer;
	}
    
    void RocketDocument::LoadScript(Rocket::Core::Stream* stream, const Rocket::Core::String& source_name) {
        if (!m_context) {
            LogError(MODULE) << "no context";
            return;
        }
        if (!m_context->Valid()) {
            LogError(MODULE) << "no valid context";
            return;
        }
        luabind::LuaVMHelperPtr helper = m_context->GetHelper();
        if (!helper) {
            LogError(MODULE) << "no helper";
            return;
        }
        lua_State* L = helper->lua;
        
        int initial_index = lua_gettop(helper->lua);
        
        m_context->GetObject(L);      /// ctx
        
        rocket_read_data data = {stream,{}};
        lua_pushcclosure(L, &luabind::lua_traceback, 0);  /// tb
        
		int res = lua_load(helper->lua,&rocket_read_func,&data,source_name.CString(),0);  /// MF tf
		
        if (res!=0) {
			LogError(MODULE) << "Failed to load script: " << source_name.CString();
            LogError(MODULE) << lua_tostring(L, -1) ;
            lua_pop(L, 3);
		} else {
			LogInfo(MODULE) << "Loaded script: " << source_name.CString();
            
            lua_pushvalue(L, -3);
            lua_setupvalue(L, -2, 1);  /* set it as 1st upvalue of loaded chunk */
            
            //
            int res = lua_pcall(L, 0, 0, -2);
            if (res) {
                LogError(MODULE) << "pcall : " << res;
                LogError(MODULE) << lua_tostring(L, -1) ;
                lua_pop(L, 3);
                return;
            }
            int crnt_top = lua_gettop(L);
            lua_pop(L, crnt_top-initial_index);
		}

    }
}
