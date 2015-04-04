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
#include "sb_event.h"


SB_META_BEGIN_KLASS_BIND(Sandbox::Thread)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ThreadsMgr)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(AddThread)
SB_META_METHOD(RemoveThread)
SB_META_METHOD(Clear)
SB_META_PROPERTY_RW_DEF(DropEmpty)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::PermanentThreadsMgr)
SB_META_CONSTRUCTOR(())
SB_META_END_KLASS_BIND()


SB_META_DECLARE_KLASS(Sandbox::Event, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Event)
SB_META_METHOD(Emmit)
SB_META_END_KLASS_BIND()


namespace Sandbox {
    static const char* const LuaEventModule = "Sanbox:LuaEvent";
    
  	class LuaEvent : public Event {
	public:
        typedef sb::intrusive_ptr<LuaEvent> Ptr;
        explicit LuaEvent(luabind::LuaVMHelperWeakPtr ptr) : m_ref(ptr) {}
		~LuaEvent() {
		}
		void SetFunction(lua_State* L) {
			m_ref.SetObject(L);
		}
		void Emmit() {
			if (m_ref.Valid()) {
				if ( luabind::LuaVMHelperPtr lua = m_ref.GetHelper()) {
					lua_State* L = lua->lua;
                    LUA_CHECK_STACK(0)
					sb_assert(L);
					m_ref.GetObject(L);
					sb_assert(lua_isfunction(L,-1));
					int res = lua_pcall(L, 0, 0, 0);
					if (res) {
						LogError(LuaEventModule) << " Failed script event emmit  " ;
                        LogError(LuaEventModule) << lua_tostring(L, -1) ;
					}
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
			sb::intrusive_ptr<LuaEvent> e = sb::intrusive_ptr<LuaEvent>(new LuaEvent(helper));
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
            luabind::stack<sb::intrusive_ptr<LuaEvent> >::push(L, e);
			return 1;
		}
	private:
        luabind::LuaReference	m_ref;
	};

    namespace luabind {
        
        EventPtr stack<EventPtr>::get(lua_State* L, int idx) {
            if (lua_isfunction(L, idx)) {
                return LuaEvent::construct_from_lua_function(L,idx);
            }
            return EventPtr(stack<Event*>::get(L, idx));
        }
        
    }
    
    
}

SB_META_DECLARE_KLASS(Sandbox::LuaEvent, Event)
SB_META_BEGIN_KLASS_BIND(Sandbox::LuaEvent)
bind(constructor(&Sandbox::LuaEvent::constructor_func));
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
        luabind::Class<LuaEvent>(lua);
    }
    
}