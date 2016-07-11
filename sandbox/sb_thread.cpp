//
//  sb_thread.cpp
//  backgammon-osx
//
//  Created by Andrey Kunitsyn on 12/29/12.
//  Copyright (c) 2012 Andrey Kunitsyn. All rights reserved.
//

#include "sb_thread.h"
#include "sb_lua.h"
#include <sbstd/sb_platform.h>

SB_META_DECLARE_OBJECT(Sandbox::Thread, Sandbox::meta::object)

namespace Sandbox {
    
    Thread::Thread() {
        
    }
    
    Thread::~Thread() {
        
    }
    
    
    static const char* const LuaThreadModule = "Sanbox:LuaThread";
	
    void LuaThread::SetThread(lua_State* L) {
            LUA_CHECK_STACK(-1)
			sb_assert(lua_isthread(L,-1));
			m_ref.SetObject( L );
		}
    bool LuaThread::Update(float dt) {
			if (m_ref.Valid()) {
				if ( luabind::LuaVMHelperPtr lua = m_ref.GetHelper()) {
					lua_State* L = lua->lua;
                    LUA_CHECK_STACK(0)
                    //LogVerbose(LuaThreadModule) << "update thread >>> " << lua_gettop(L);
                    
					sb_assert(L);
					m_ref.GetObject(L);
					if (!lua_isthread(L,-1)) {
						LogError(LuaThreadModule) << "not thread :" << luaL_typename(L,-1);
						sb_assert(lua_isthread(L,-1));
                        return true;
					}
					lua_State* th = lua_tothread(L, -1);
                    
                    g_terminate_thread = th;
                    
                    int status = lua_status(th);
                    if (status!=LUA_YIELD) {
                        //LogDebug(LuaThreadModule) << "thread " << th << " status: " << status;
                    }
					lua_pushnumber(th, dt);
					int res = lua_resume(th,0, 1);
                    
                    g_terminate_thread = 0;
                    
					if (res==LUA_YIELD) {
                        lua_pop(L,1);
                        if (status!=LUA_YIELD) {
                            //LogDebug(LuaThreadModule) << "thread " << th << " suspended: " << status;
                        }
                        //LogVerbose(LuaThreadModule) << "update thread <<< " << lua_gettop(L);
                        return false;
					}
					if (res!=LUA_OK) {
                        LogError(LuaThreadModule) << "Failed script resume  " << res;
                        if (res==LUA_ERRRUN) {
                            LogError(LuaThreadModule) << "error:" << lua_tostring(th, -1);
                            luabind::PushErrorHandler(th);
                            lua_pushvalue(th, -2);
                            lua_pcall(th, 1, 1, 0);
                            LogError(LuaThreadModule) << lua_tostring(th, -1) ;
                            lua_pop(th, 1);
                            lua_pop(L,1);
                        }
                    } else {
                        lua_pop(L,1);
                        //LogDebug(LuaThreadModule) << "thread " << th << " ended";
                    }
                    //LogVerbose(LuaThreadModule) << "update thread <<< " << lua_gettop(L);
				} else {
					LogError(LuaThreadModule) << "update on released script";
          		}
			} else {
                LogError(LuaThreadModule) << "update destroyed thread";
            }
			return true;
		}
    sb::intrusive_ptr<LuaThread> LuaThread::construct(lua_State* L,int idx) {
        if (!lua_isfunction(L,idx)) {
            char buf[128];
            sb::snprintf(buf,127,"function expected, got %s",luaL_typename(L, 2));
            luaL_argerror(L, 2, buf);
            return sb::intrusive_ptr<LuaThread>();
        }
        int stck = lua_gettop(L);
        luabind::LuaVMHelperPtr helper = luabind::GetHelper(L);
        sb_assert(helper);
        lua_State* main_state = helper->lua;
        sb_assert(lua_checkstack(main_state,3));
        sb::intrusive_ptr<LuaThread> e = sb::intrusive_ptr<LuaThread>(new LuaThread(helper));
        //sb_assert(stck==lua_gettop(L));
        lua_pushvalue(L, idx);
        if (main_state!=L) {
            lua_xmove(L, main_state, 1);			/// (1) func
        }
        lua_State* thL = lua_newthread(main_state); /// (2) th,func
        sb_assert(lua_isthread(main_state,-1));
        lua_pushvalue(main_state, -2);				/// (3) func,th,func
        lua_xmove(main_state, thL, 1);				/// (2) th,func
        e->SetThread(main_state);				/// (1) func
        lua_pop(main_state,1);
        int new_top = lua_gettop(L);
        (void)stck;
        (void)new_top;
        sb_assert(stck==new_top);
        return e;
    }
    int LuaThread::constructor_func(lua_State* L) {
            LUA_CHECK_STACK(1)
            //LogVerbose(LuaThreadModule) << "construct thread >>>> " << lua_gettop(L);
            sb::intrusive_ptr<LuaThread> res = construct(L, 2);
            luabind::stack<sb::intrusive_ptr<LuaThread> >::push(L, res);
        	return 1;
    }

    
}
