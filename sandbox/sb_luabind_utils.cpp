//
//  sb_luabind_math.cpp
//  YinYang
//
//  Created by Андрей Куницын on 5/13/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "luabind/sb_luabind.h"
#include "meta/sb_meta.h"
#include "utils/sb_base64.h"
#include "utils/sb_md5.h"

namespace Sandbox {
    struct utils {
        
    };
}


SB_META_BEGIN_KLASS_BIND(Sandbox::utils)

SB_META_END_KLASS_BIND()


namespace Sandbox {
    
    void register_math( lua_State* lua ) {
        luabind::Namespace ns(lua,"Sandbox.utils");
        ns( Sandbox::meta::static_method( "Md5Sum" , &Sandbox::MD5Sum) );
        ns( Sandbox::meta::static_method( "Base64Decode" , &Sandbox::Base64Decode) );
        ns( Sandbox::meta::static_method( "Base64Encode" , &Sandbox::Base64Encode) );
    }
    
}

