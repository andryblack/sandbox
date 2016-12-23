//
//  sb_luabind_controller.cpp
//  YinYang
//
//  Created by Андрей Куницын on 7/9/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "luabind/sb_luabind.h"
#include "meta/sb_meta.h"
#include "sb_processor.h"
#include "sb_controllers_transform.h"
#include "sb_controller_split.h"
#include "sb_controller_bidirect.h"
#include "sb_controller_phase.h"
#include "sb_controller_map.h"
#include "sb_controller_elastic.h"
#include "sb_controllers_color.h"
#include "sb_animation.h"



SB_META_BEGIN_KLASS_BIND(Sandbox::Controller)
SB_META_METHOD(Set)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Processor, Sandbox::Thread)
SB_META_BEGIN_KLASS_BIND(Sandbox::Processor)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(Start)
SB_META_METHOD(Stop)
SB_META_PROPERTY_RW(Controller,GetController,SetController)
SB_META_PROPERTY_RW_DEF(OnEnd)
SB_META_PROPERTY_RW(Time,GetTime,SetTime)
SB_META_PROPERTY_RW(Loop,GetLoop,SetLoop)
SB_META_PROPERTY_RW(Inverted,GetInverted,SetInverted)
SB_META_PROPERTY_RO(Completed, Completed)
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

SB_META_DECLARE_KLASS(Sandbox::ControllerOffsets, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerOffsets)
SB_META_CONSTRUCTOR((ControllerPtr))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerElastic, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerElastic)
SB_META_CONSTRUCTOR((ControllerPtr))
SB_META_PROPERTY_RW_DEF(Hard)
SB_META_PROPERTY_RW_DEF(End)
SB_META_PROPERTY_RW_DEF(Phases)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerSinus, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerSinus)
SB_META_CONSTRUCTOR((ControllerPtr))
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

SB_META_DECLARE_KLASS(Sandbox::ControllerTranslateX, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerTranslateX)
SB_META_CONSTRUCTOR((const Sandbox::TransformModificatorPtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerTranslateY, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerTranslateY)
SB_META_CONSTRUCTOR((const Sandbox::TransformModificatorPtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerTranslateMove, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerTranslateMove)
SB_META_CONSTRUCTOR((const Sandbox::TransformModificatorPtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_PROPERTY_RW_DEF(Delta)
SB_META_END_KLASS_BIND()


SB_META_DECLARE_KLASS(Sandbox::ControllerPos, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerPos)
SB_META_CONSTRUCTOR((const Sandbox::SceneObjectWithPositionPtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerPosMove, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerPosMove)
SB_META_CONSTRUCTOR((const Sandbox::SceneObjectWithPositionPtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_PROPERTY_RW_DEF(Delta)
SB_META_END_KLASS_BIND()


SB_META_DECLARE_KLASS(Sandbox::ControllerColor, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerColor)
SB_META_CONSTRUCTOR((const Sandbox::ColorModificatorPtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerSpriteColor, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerSpriteColor)
SB_META_CONSTRUCTOR((const Sandbox::ColorizedSpritePtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerAlpha, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerAlpha)
SB_META_CONSTRUCTOR((const Sandbox::ColorModificatorPtr&))
SB_META_PROPERTY_RW_DEF(Begin)
SB_META_PROPERTY_RW_DEF(End)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ControllerSpriteAlpha, Controller)
SB_META_BEGIN_KLASS_BIND(Sandbox::ControllerSpriteAlpha)
SB_META_CONSTRUCTOR((const Sandbox::ColorizedSpritePtr&))
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
SB_META_METHOD(AddSync)
SB_META_METHOD(Randomize)
SB_META_PROPERTY_RW_DEF(OnEnd)
SB_META_PROPERTY_RW_DEF(Frame)
SB_META_PROPERTY_RW_DEF(FrameTime)
SB_META_PROPERTY_RO(PlayedOnce, PlayedOnce);
SB_META_END_KLASS_BIND()

namespace Sandbox {
    

    void register_controller( lua_State* lua ) {
        luabind::Class<Controller>(lua);
        luabind::Class<Processor>(lua);
        luabind::Class<ControllerSplit>(lua);
        luabind::Class<ControllerAngle>(lua);
        luabind::Class<ControllerScale>(lua);
        luabind::Class<ControllerScaleX>(lua);
        luabind::Class<ControllerScaleY>(lua);
        luabind::Class<ControllerBidirect>(lua);
        luabind::Class<ControllerPhase>(lua);
        luabind::Class<ControllerOffsets>(lua);
        luabind::Class<ControllerMap>(lua);
        luabind::Class<ControllerTranslate>(lua);
        luabind::Class<ControllerTranslateX>(lua);
        luabind::Class<ControllerTranslateY>(lua);
        luabind::Class<ControllerTranslateMove>(lua);
        luabind::Class<ControllerPos>(lua);
        luabind::Class<ControllerPosMove>(lua);
        luabind::Class<ControllerElastic>(lua);
        luabind::Class<ControllerSinus>(lua);
        luabind::Class<ControllerColor>(lua);
        luabind::Class<ControllerAlpha>(lua);
        luabind::Class<ControllerSpriteColor>(lua);
        luabind::Class<ControllerSpriteAlpha>(lua);
        luabind::Class<Animation>(lua);
        luabind::Class<AnimationData>(lua);
    }
    
}
