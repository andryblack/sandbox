//
//  sb_luabind_controller.cpp
//  YinYang
//
//  Created by Андрей Куницын on 7/9/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "luabind/sb_luabind.h"
#include "meta/sb_meta.h"
#include "sb_lua.h"
#include "sb_thread.h"
#include "sb_threads_mgr.h"
#include "sb_processor.h"
#include "sb_controllers_transform.h"
#include "sb_controller_split.h"
#include "sb_controller_bidirect.h"
#include "sb_controller_phase.h"
#include "sb_controller_map.h"
#include "sb_controller_elastic.h"
#include "sb_controllers_color.h"

SB_META_DECLARE_KLASS(Sandbox::Thread, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Thread)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ThreadsMgr, Thread)
SB_META_BEGIN_KLASS_BIND(Sandbox::ThreadsMgr)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(AddThread)
SB_META_METHOD(RemoveThread)
SB_META_METHOD(Clear)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Controller, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Controller)
SB_META_METHOD(Set)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Event, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Event)
SB_META_METHOD(Emmit)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Processor, Thread)
SB_META_BEGIN_KLASS_BIND(Sandbox::Processor)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(Start)
SB_META_METHOD(Stop)
SB_META_PROPERTY_RW(Controller,GetController,SetController)
SB_META_PROPERTY_RW(EndEvent,GetEndEvent,SetEndEvent)
SB_META_PROPERTY_RW(Time,GetTime,SetTime)
SB_META_PROPERTY_RW(Loop,GetLoop,SetLoop)
SB_META_PROPERTY_RW(Inverted,GetInverted,SetInverted)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerSplit, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerSplit)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(Reserve)
SB_META_METHOD(AddController)
SB_META_METHOD(RemoveController)
SB_META_METHOD(Clear)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerAngle, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerAngle)
SB_META_CONSTRUCTOR((ContainerTransformPtr))
SB_META_PROPERTY_RW(Begin,GetBegin,SetBegin)
SB_META_PROPERTY_RW(End,GetEnd,SetEnd)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerScale, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerScale)
SB_META_CONSTRUCTOR((ContainerTransformPtr))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerScaleX, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerScaleX)
SB_META_CONSTRUCTOR((ContainerTransformPtr))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerScaleY, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerScaleY)
SB_META_CONSTRUCTOR((ContainerTransformPtr))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerBidirect, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerBidirect)
SB_META_CONSTRUCTOR((ControllerPtr))
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerPhase, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerPhase)
SB_META_CONSTRUCTOR((ControllerPtr))
SB_META_PROPERTY_RW_DEF(Phase)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerElastic, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerElastic)
SB_META_CONSTRUCTOR((ControllerPtr))
SB_META_PROPERTY_RW_DEF(Hard)
SB_META_PROPERTY_RW_DEF(End)
SB_META_PROPERTY_RW_DEF(Amplitude)
SB_META_PROPERTY_RW_DEF(Phases)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerMap, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerMap)
SB_META_CONSTRUCTOR((ControllerPtr))
SB_META_METHOD(SetPoint)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerTranslate, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerTranslate)
SB_META_CONSTRUCTOR((const Sandbox::ContainerTransformPtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerColor, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerColor)
SB_META_CONSTRUCTOR((const Sandbox::ContainerColorPtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerAlpha, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerAlpha)
SB_META_CONSTRUCTOR((const Sandbox::ContainerColorPtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()


namespace Sandbox {
    static const char* const LuaEventModule = "Sanbox:LuaEvent";
    
  	class LuaEvent : public Event {
	public:

        explicit LuaEvent(LuaVMHelperWeakPtr ptr) : m_ref(ptr) {}
		~LuaEvent() {
		}
		void SetFunction(lua_State* L) {
			m_ref.SetObject(L);
		}
		void Emmit() {
			if (m_ref.Valid()) {
				if ( LuaVMHelperPtr lua = m_ref.GetHelper()) {
					lua_State* L = lua->lua->GetVM();
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
		static int constructor_func(lua_State* L) {
			if (!lua_isfunction(L,2)) {
                luabind::lua_argerror(L,2,"function",0);
				return 0;
			}
 			LuaVM* lua = LuaVM::GetInstance(L);
            if (!lua) {
                lua_pushstring(L, "error state");
                lua_error(L);
                return 0;
            }
			lua_State* main_state = lua->GetVM();
			sb::shared_ptr<LuaEvent> e = sb::shared_ptr<LuaEvent>(new LuaEvent(lua->GetHelper()));
			lua_pushvalue(L, 2);
			if (main_state!=L) {
				lua_xmove(L, main_state, 1);
			}
			e->SetFunction(main_state);
            luabind::stack<sb::shared_ptr<LuaEvent> >::push(L, e);
			return 1;
		}
	private:
		LuaReference	m_ref;
	};

    
    static const char* const LuaThreadModule = "Sanbox:LuaThread";
	class LuaThread : public Thread {
	public:
		explicit LuaThread(LuaVMHelperWeakPtr ptr) : m_ref(ptr) {}
		~LuaThread() {
		}
		void SetThread(lua_State* L) {
			sb_assert(lua_isthread(L,-1));
			m_ref.SetObject( L );
		}
		bool Update(float dt) {
			if (m_ref.Valid()) {
				if ( LuaVMHelperPtr lua = m_ref.GetHelper()) {
					lua_State* L = lua->lua->GetVM();
					sb_assert(L);
					m_ref.GetObject(L);
					if (!lua_isthread(L,-1)) {
						LogError(LuaThreadModule) << "not thread :" << luaL_typename(L,-1);
						sb_assert(lua_isthread(L,-1));
                        return false;
					} 
					lua_State* th = lua_tothread(L, -1);
					lua_pushnumber(th, dt);
					int res = lua_resume(th,L, 1);
					lua_pop(L,1);
					if (res==LUA_YIELD) {
						return false;
					}
					if (res) {
                        lua_pushcclosure(th, &luabind::lua_traceback, 0);
                        lua_pushvalue(th, -2);
                        lua_pcall(th, 1, 1, 0);
						LogError(LuaThreadModule) << "Failed script resume  " ;
                        LogError(LuaThreadModule) << lua_tostring(th, -1) ;
                        lua_pop(th, 2);
					}
				} else {
					LogError(LuaThreadModule) << "update on released script";
				}
			}
			return true;
		}
		static int constructor_func(lua_State* L) {
			int stck = lua_gettop(L);
			if (!lua_isfunction(L,2)) {
				char buf[128];
				::snprintf(buf,127,"function expected, got %s",luaL_typename(L, 2));
				luaL_argerror(L, 2, buf);
				return 0;
			}
			//int stck_L = lua_gettop(L);
			LuaVM* lua = LuaVM::GetInstance(L);
			lua_State* main_state = lua->GetVM();
			sb_assert(lua_checkstack(main_state,3));
			sb::shared_ptr<LuaThread> e = sb::shared_ptr<LuaThread>(new LuaThread(lua->GetHelper()));
			//sb_assert(stck==lua_gettop(L));
			lua_pushvalue(L, 2);						
			if (main_state!=L) {
				lua_xmove(L, main_state, 1);			/// (1) func
			}
			lua_State* thL = lua_newthread(main_state); /// (2) th,func
			sb_assert(lua_isthread(main_state,-1));
			lua_pushvalue(main_state, -2);				/// (3) func,th,func
			lua_xmove(main_state, thL, 1);				/// (2) th,func
			int res = lua_resume(thL,main_state, 0);
			if (res==LUA_YIELD) {
				e->SetThread(main_state);				/// (1) func
			} else {
				if (res!=0) {
					LogError(LuaThreadModule) <<"Failed thread run  ";
                    LogError(LuaThreadModule) << lua_tostring(thL, -1);
				}
				lua_pop(main_state,1);					/// (1) func
			}
			lua_pop(main_state,1);
			int new_top = lua_gettop(L);
			(void)stck;
			(void)new_top;
			sb_assert(stck==new_top);
            luabind::stack<sb::shared_ptr<LuaThread> >::push(L, e);
			return 1;
		}
	private:
		LuaReference m_ref;
	};

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
    

    void register_controller( luabind::LuaRegistrator& lua ) {
        lua.klass<Thread>();
        lua.klass<LuaThread>();
        lua.klass<ThreadsMgr>();
        lua.klass<Controller>();
        lua.klass<Event>();
        lua.klass<LuaEvent>();
        lua.klass<Processor>();
        lua.klass<ControllerSplit>();
        lua.klass<ControllerAngle>();
        lua.klass<ControllerScale>();
        lua.klass<ControllerScaleX>();
        lua.klass<ControllerScaleY>();
        lua.klass<ControllerBidirect>();
        lua.klass<ControllerPhase>();
        lua.klass<ControllerMap>();
        lua.klass<ControllerTranslate>();
        lua.klass<ControllerElastic>();
        lua.klass<ControllerColor>();
        lua.klass<ControllerAlpha>();
    }
    
}