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
            static const type_info ci = {"void",0,0,0,0};
            return &ci;
        }
        
    }
    
}

SB_META_DECLARE_POD_TYPE(bool)
SB_META_DECLARE_POD_TYPE(float)
SB_META_DECLARE_POD_TYPE(double)
SB_META_DECLARE_POD_TYPE(int)
SB_META_DECLARE_POD_TYPE(long)
SB_META_DECLARE_POD_TYPE(short)
