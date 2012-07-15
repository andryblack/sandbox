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
        
        template <> 
        const type_info* type<void>::info() {
            static const type_info ci = {"void",0,0};
            return &ci;
        }
        template <> const type_info* type<object>::info() {
            static const type_info_parent parents[] = {
                {
                    type<void>::info(),
                    &cast_helper<object,void>::raw,
                    &cast_helper<object,void>::shared
                },
                { 0, 0, 0 }
            };
            static const type_info ti = { 
                "Sandbox::meta::object", 
                sizeof(object),
                parents
            };
            return &ti;
        }
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
