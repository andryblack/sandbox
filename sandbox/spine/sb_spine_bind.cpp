#include "sb_spine_bind.h"
#include "sb_spine_animation.h"
#include "sb_spine_data.h"


#include "luabind/sb_luabind.h"
#include "meta/sb_meta.h"

SB_META_BEGIN_KLASS_BIND(Sandbox::SpineData)
SB_META_STATIC_METHOD(Load)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::SpineAnimation)
SB_META_CONSTRUCTOR((Sandbox::SpineDataPtr))
SB_META_METHOD(StartAnimation)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::SpineSceneObject)
SB_META_CONSTRUCTOR((Sandbox::SpineAnimationPtr))
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    void register_spine( lua_State* lua ) {
        luabind::Class<SpineData>(lua);
        luabind::Class<SpineAnimation>(lua);
        luabind::Class<SpineSceneObject>(lua);
    }
    
}