//
//  sb_luabind_wrapper.cpp
//  YinYang
//
//  Created by Андрей Куницын on 7/15/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "sb_luabind_wrapper.h"

SB_META_DECLARE_KLASS(Sandbox::luabind::wrapper, void);

namespace Sandbox {
    namespace luabind {
     
        const meta::type_info* const* wrapper::get_parents() {
            static const meta::type_info* const p[] = {
                meta::type<wrapper>::info(),
                0
            };
            return p;
        }
    }
}