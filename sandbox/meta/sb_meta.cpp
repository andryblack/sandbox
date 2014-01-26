//
//  sb_meta.cpp
//  YinYang
//
//  Created by Андрей Куницын on 6/5/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "sb_meta.h"


namespace Sandbox {
    
    
    namespace meta {
        
        static const type_info* void_ti = 0;
        
        template <> 
        const type_info* type<void>::private_info = void_ti;
        
        
        static const type_info object_ti = {
            "Sandbox::meta::object",
            sizeof(object),
            {
                0,
                0,
                0
            }
        };
        template <> const type_info* type<object>::private_info = &object_ti;
        
        const type_info* object::get_static_type_info() {
            return type<object>::info();
        }
    }
    
}


SB_META_DECLARE_POD_TYPE(bool)
SB_META_DECLARE_POD_TYPE(float)
SB_META_DECLARE_POD_TYPE(double)
SB_META_DECLARE_POD_TYPE(int)
SB_META_DECLARE_POD_TYPE(long)
SB_META_DECLARE_POD_TYPE(short)
//SB_META_DECLARE_POD_TYPE(sb::string)
