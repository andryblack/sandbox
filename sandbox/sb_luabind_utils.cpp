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
        
    void register_utils( lua_State* lua ) {
        luabind::Namespace ns(lua,"Sandbox.utils");
        ns( Sandbox::meta::static_method( "MD5" , &Sandbox::MD5Sum) );
        ns( Sandbox::meta::static_method( "MD5File", &Sandbox::MD5SumFile));
        ns( Sandbox::meta::static_method( "Base64Decode" , &Sandbox::lua_Base64Decode) );
        ns( Sandbox::meta::static_method( "Base64Encode" , &Sandbox::Base64Encode) );
    }
    
}

