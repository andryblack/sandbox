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
        m_time += dt;
        if (m_time < m_pause) {
            return !m_ref.Valid();
        }
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
                int rm_stack = 0;
                if (status!=LUA_YIELD) {
                    //LogDebug(LuaThreadModule) << "thread " << th << " status: " << status;
                    rm_stack = -1;
                }
#ifdef SB_DEBUG
                ::Sandbox::luabind::lua_stack_check sck_thread_state(th,status == LUA_YIELD ? 0 : -1);
#endif

                lua_checkstack(th, 1);
                int top = lua_gettop(th);
                top += rm_stack;
                lua_pushnumber(th, m_time);
                int res = lua_resume(th,0, 1);
                m_time = 0.0f;
                m_pause = 0.0f;
                g_terminate_thread = 0;
                
                if (res==LUA_YIELD) {
                    int new_top = lua_gettop(th);
                    if (new_top > top) {
                        if (lua_isnumber(th, -1))
                            m_pause = lua_tonumber(th, -1);
                    }
                    lua_settop(th, top);
                    
                    lua_pop(L,1);
                    
                    sb_assert(sck_thread_state.check_state());
                    
                    if (status!=LUA_YIELD) {
                        //LogDebug(LuaThreadModule) << "thread " << th << " suspended: " << status;
                    }
                    //LogVerbose(LuaThreadModule) << "update thread <<< " << lua_gettop(L);
                    return false;
                }
                if (res!=LUA_OK) {
                    LogError(LuaThreadModule) << "Failed script resume  " << res << " " << lua_gettop(L);
                    if (res==LUA_ERRRUN) {
                        luabind::PushErrorHandler(L); // th,eh
                        lua_pushvalue(L,-2);          // th,eh,th
                        lua_pcall(L, 1, 1,0);         // th,err
                        LogError(LuaThreadModule) << luabind::stack<const char*>::get(L,-1);
                        lua_pop(L,1);
                    }
                }
                lua_pop(L,1);
                lua_settop(th, top);
                
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
        LUA_CHECK_STACK(0)
        if (!lua_isfunction(L,idx)) {
            char buf[128];
            sb::snprintf(buf,127,"function expected, got %s",luaL_typename(L, 2));
            luaL_argerror(L, 2, buf);
            return sb::intrusive_ptr<LuaThread>();
        }
        luabind::LuaVMHelperPtr helper = luabind::GetHelper(L);
        sb_assert(helper);
        lua_State* main_state = helper->lua;
#ifdef SB_DEBUG
        ::Sandbox::luabind::lua_stack_check sck_main_state(main_state,0);
#endif
        
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
        return e;
    }
    int LuaThread::constructor_func(lua_State* L) {
            LUA_CHECK_STACK(1)
            //LogVerbose(LuaThreadModule) << "construct thread >>>> " << lua_gettop(L);
            sb::intrusive_ptr<LuaThread> res = construct(L, 2);
            luabind::stack<sb::intrusive_ptr<LuaThread> >::push(L, res);
        	return 1;
    }
    void LuaThread::Clear() {
        m_ref.Reset();
    }
    
}
