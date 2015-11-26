#include "sb_skelet_bind.h"
#include "sb_skelet_data.h"
#include "sb_skelet_controller.h"
#include "sb_skelet_object.h"



#include "luabind/sb_luabind.h"
#include "meta/sb_meta.h"

SB_META_BEGIN_KLASS_BIND(Sandbox::SkeletonData)
SB_META_STATIC_METHOD(Load)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::SkeletController)
SB_META_CONSTRUCTOR((Sandbox::SkeletonDataPtr))
SB_META_METHOD(StartAnimation)
SB_META_METHOD(AddAnimation)
SB_META_METHOD(AddObject)
SB_META_PROPERTY_RW_DEF(EndEvent)
SB_META_PROPERTY_RO(IsComplete, IsComplete)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::SkeletObject)
SB_META_CONSTRUCTOR(())
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    void register_skelet( lua_State* lua ) {
        luabind::Class<SkeletonData>(lua);
        luabind::Class<SkeletController>(lua);
        luabind::Class<SkeletObject>(lua);
    }
    
}