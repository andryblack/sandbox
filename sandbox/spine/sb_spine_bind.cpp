#include "sb_spine_bind.h"
#include "sb_spine_animation.h"
#include "sb_spine_data.h"


#include "luabind/sb_luabind.h"
#include "meta/sb_meta.h"

SB_META_BEGIN_KLASS_BIND(Sandbox::SpineData)
SB_META_STATIC_METHOD(Load)
SB_META_METHOD(SetMix)
SB_META_METHOD(SetDefaultMix)
SB_META_METHOD(GetSlotsCount)
SB_META_METHOD(GetSlotName)
SB_META_METHOD(SetSlotAttribute)
SB_META_METHOD(GetSlotImage)
SB_META_METHOD(GetAttachmentImage)
SB_META_METHOD(GetAnimationEvents)
SB_META_METHOD(PreloadTextures)
SB_META_PROPERTY_RO(Size, GetSize)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::SpineAnimation)
SB_META_CONSTRUCTOR((Sandbox::SpineDataPtr))
SB_META_METHOD(StartAnimation)
SB_META_METHOD(SetAnimation)
SB_META_METHOD(AddAnimation)
SB_META_METHOD(SetTime)
SB_META_METHOD(HasAnimation)
SB_META_METHOD(GetSlotColor)
SB_META_PROPERTY_RW_DEF(OnEnd)
SB_META_PROPERTY_RW_DEF(OnEvent)
SB_META_PROPERTY_RO(CurrentAnimationLength, GetCurrentAnimationLength)
SB_META_PROPERTY_RO(IsComplete, IsComplete)
SB_META_METHOD(CalcBoundingBox)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::SpineSceneObject)
SB_META_CONSTRUCTOR((Sandbox::SpineAnimationPtr))
SB_META_METHOD(SetAttachement)
SB_META_METHOD(RemoveAttachement)
SB_META_METHOD(GetAttachment)
SB_META_METHOD(CheckHit)
SB_META_METHOD(CheckSlotHit)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    void register_spine( lua_State* lua ) {
        luabind::Class<SpineData>(lua);
        luabind::Class<SpineAnimation>(lua);
        luabind::Class<SpineSceneObject>(lua);
    }
    
}
