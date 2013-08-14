//
//  sb_rocket_bind.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/12/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_rocket_bind.h"
#include "sb_rocket_context.h"
#include "sb_rocket_document.h"

SB_META_DECLARE_KLASS(Rocket::Core::Element, void)
SB_META_BEGIN_KLASS_BIND(Rocket::Core::Element)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Rocket::Core::ElementDocument, Rocket::Core::Element)
SB_META_BEGIN_KLASS_BIND(Rocket::Core::ElementDocument)
SB_META_METHOD(Show)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::RocketDocument)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::RocketContext, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::RocketContext)
SB_META_METHOD(LoadRocketDocument)
SB_META_METHOD(RegisterFont)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    void RocketBind(lua_State* L) {
        luabind::ExternClass<Rocket::Core::Element>(L);
        luabind::ExternClass<Rocket::Core::ElementDocument>(L);
        luabind::ExternClass<RocketDocument>(L);
        luabind::ExternClass<RocketContext>(L);
    }
}
