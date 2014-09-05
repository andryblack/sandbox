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
#include "sb_processor.h"
#include "sb_controllers_transform.h"
#include "sb_controller_split.h"
#include "sb_controller_bidirect.h"
#include "sb_controller_phase.h"
#include "sb_controller_map.h"
#include "sb_controller_elastic.h"
#include "sb_controllers_color.h"
#include "sb_animation.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

SB_META_BEGIN_KLASS_BIND(Sandbox::Thread)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ThreadsMgr)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(AddThread)
SB_META_METHOD(RemoveThread)
SB_META_METHOD(Clear)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::PermanentThreadsMgr)
SB_META_CONSTRUCTOR(())
SB_META_END_KLASS_BIND()

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
SB_META_CONSTRUCTOR((TransformModificatorPtr))
SB_META_PROPERTY_RW(Begin,GetBegin,SetBegin)
SB_META_PROPERTY_RW(End,GetEnd,SetEnd)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerScale, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerScale)
SB_META_CONSTRUCTOR((TransformModificatorPtr))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerScaleX, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerScaleX)
SB_META_CONSTRUCTOR((TransformModificatorPtr))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerScaleY, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerScaleY)
SB_META_CONSTRUCTOR((TransformModificatorPtr))
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
SB_META_CONSTRUCTOR((const Sandbox::TransformModificatorPtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerColor, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerColor)
SB_META_CONSTRUCTOR((const Sandbox::ColorModificatorPtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerAlpha, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerAlpha)
SB_META_CONSTRUCTOR((const Sandbox::ColorModificatorPtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::AnimationData, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::AnimationData)
SB_META_CONSTRUCTOR(())
SB_META_PROPERTY_RW_DEF(Speed)
SB_META_PROPERTY_RW_DEF(LoopFrame)
SB_META_METHOD(Reserve)
SB_META_METHOD(AddFrame)
SB_META_METHOD(GetImage)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::Animation)
SB_META_CONSTRUCTOR((AnimationDataPtr))
SB_META_METHOD(Start)
SB_META_METHOD(ClearSprites)
SB_META_METHOD(Stop)
SB_META_METHOD(AddSprite)
SB_META_PROPERTY_RO(PlayedOnce, PlayedOnce);
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
    

    void register_controller( lua_State* lua ) {
        luabind::Class<Thread>(lua);
        luabind::Class<LuaThread>(lua);
        luabind::Class<ThreadsMgr>(lua);
        luabind::Class<PermanentThreadsMgr>(lua);
        luabind::Class<Controller>(lua);
        luabind::Class<Event>(lua);
        luabind::Class<LuaEvent>(lua);
        luabind::Class<Processor>(lua);
        luabind::Class<ControllerSplit>(lua);
        luabind::Class<ControllerAngle>(lua);
        luabind::Class<ControllerScale>(lua);
        luabind::Class<ControllerScaleX>(lua);
        luabind::Class<ControllerScaleY>(lua);
        luabind::Class<ControllerBidirect>(lua);
        luabind::Class<ControllerPhase>(lua);
        luabind::Class<ControllerMap>(lua);
        luabind::Class<ControllerTranslate>(lua);
        luabind::Class<ControllerElastic>(lua);
        luabind::Class<ControllerColor>(lua);
        luabind::Class<ControllerAlpha>(lua);
        luabind::Class<Animation>(lua);
        luabind::Class<AnimationData>(lua);
    }
    
}