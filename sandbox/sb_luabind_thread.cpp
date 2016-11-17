//
//  sb_luabind_controller.cpp
//  YinYang
//
//  Created by Андрей Куницын on 7/9/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "luabind/sb_luabind.h"
#include "meta/sb_meta.h"
#include "sb_thread.h"
#include "sb_threads_mgr.h"
//#include "sb_event.h"
#include "sb_signal.h"


SB_META_BEGIN_KLASS_BIND(Sandbox::Thread)
SB_META_METHOD(Update)
SB_META_METHOD(Clear)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ThreadsMgr)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(AddThread)
SB_META_METHOD(RemoveThread)
SB_META_METHOD(Clear)
SB_META_PROPERTY_RW_DEF(DropEmpty)
SB_META_PROPERTY_RW_DEF(Speed)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::PermanentThreadsMgr)
SB_META_CONSTRUCTOR(())
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Event, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Event)
SB_META_METHOD(SetInt)
SB_META_METHOD(GetInt)
SB_META_METHOD(SetString)
SB_META_METHOD(GetString)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Signal, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Signal)
SB_META_METHOD_S(Emmit,void(Sandbox::Signal::*)(const EventPtr&))
SB_META_METHOD(Clear)
SB_META_END_KLASS_BIND()


namespace Sandbox {
    static const char* const LuaEventModule = "Sanbox:LuaEvent";
    
  	class LuaSignal : public Signal {
	public:
        typedef sb::intrusive_ptr<LuaSignal> Ptr;
        explicit LuaSignal(luabind::LuaVMHelperWeakPtr ptr) : m_ref(ptr) {}
		~LuaSignal() {
		}
		void SetFunction(lua_State* L) {
			m_ref.SetObject(L);
		}
		void Emmit(const EventPtr& e) {
			if (m_ref.Valid()) {
				if ( luabind::LuaVMHelperPtr lua = m_ref.GetHelper()) {
					lua_State* L = lua->lua;
                    LUA_CHECK_STACK(0)
					sb_assert(L);
                    luabind::PushErrorHandler(L);
					m_ref.GetObject(L);
					sb_assert(lua_isfunction(L,-1));
                    luabind::stack<EventPtr>::push(L, e);
					int res = lua_pcall(L, 1, 0, -3);
					if (res) {
						LogError(LuaEventModule) << " Failed script event emmit  " << res;
                        LogError(LuaEventModule) << lua_tostring(L, -1);
                        lua_pop(L, 2);
                        return;
					}
                    lua_pop(L, 1);
				} else {
					LogError(LuaEventModule) <<" call emmit on released script" ;
				}
			}
		}
        static Ptr construct_from_lua_function(lua_State* L,int idx) {
            if (!lua_isfunction(L,idx)) {
                luabind::lua_argerror(L,idx,"function",0);
				return Ptr();
			}
            luabind::LuaVMHelperPtr helper = luabind::GetHelper(L);
            if (!helper) {
                lua_pushstring(L, "error state");
                lua_error(L);
                return Ptr();
            }
			lua_State* main_state = helper->lua;
			sb::intrusive_ptr<LuaSignal> e = sb::intrusive_ptr<LuaSignal>(new LuaSignal(helper));
			lua_pushvalue(L, idx);
			if (main_state!=L) {
				lua_xmove(L, main_state, 1);
			}
			e->SetFunction(main_state);
            return e;
        }
		static int constructor_func(lua_State* L) {
            LUA_CHECK_STACK(1)
			Ptr e = construct_from_lua_function(L, 2);
            luabind::stack<sb::intrusive_ptr<LuaSignal> >::push(L, e);
			return 1;
		}
        virtual void Clear(){
            m_ref.Reset();
        }
	private:
        luabind::LuaReference	m_ref;
	};

    namespace luabind {
        
        SignalPtr stack<SignalPtr>::get(lua_State* L, int idx) {
            if (lua_isfunction(L, idx)) {
                return LuaSignal::construct_from_lua_function(L,idx);
            }
            return SignalPtr(stack<Signal*>::get(L, idx));
        }
        
    }
    
    
}

SB_META_DECLARE_KLASS(Sandbox::LuaSignal, Signal)
SB_META_BEGIN_KLASS_BIND(Sandbox::LuaSignal)
bind(constructor(&Sandbox::LuaSignal::constructor_func));
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::LuaThread, Thread)
SB_META_BEGIN_KLASS_BIND(Sandbox::LuaThread)
bind(constructor(&Sandbox::LuaThread::constructor_func));
SB_META_END_KLASS_BIND()

namespace Sandbox {
    

    void register_thread( lua_State* lua ) {
        luabind::Class<Thread>(lua);
        luabind::Class<LuaThread>(lua);
        luabind::Class<ThreadsMgr>(lua);
        luabind::Class<PermanentThreadsMgr>(lua);
        luabind::Class<Event>(lua);
        luabind::Class<Signal>(lua);
        luabind::Class<LuaSignal>(lua);
    }
    
}
