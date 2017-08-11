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
#include "utils/sb_url.h"

#include "sb_data.h"


namespace Sandbox {
    
    int lua_ZlibUncompress(lua_State* L);
    int lua_ZlibCompress(lua_State* L);
        
    void register_utils( lua_State* lua ) {
        luabind::Namespace ns(lua,"Sandbox.utils");
        ns( Sandbox::meta::static_method( "MD5" , &Sandbox::MD5Sum) );
        ns( Sandbox::meta::static_method( "MD5Hash", &Sandbox::MD5Hash) );
        ns( Sandbox::meta::static_method( "MD5File", &Sandbox::MD5SumFile));
        ns( Sandbox::meta::static_method( "Base64Decode" , &Sandbox::lua_Base64Decode) );
        ns( Sandbox::meta::static_method( "Base64Encode" , &Sandbox::lua_Base64Encode) );
        ns( Sandbox::meta::static_method( "UrlEncode", &Sandbox::UrlEncode ));
        ns( Sandbox::meta::static_method( "ZlibUncompress" , &Sandbox::lua_ZlibUncompress) );
        ns( Sandbox::meta::static_method( "ZlibCompress" , &Sandbox::lua_ZlibCompress) );
    }
    
}

