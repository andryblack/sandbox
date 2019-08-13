//
//  sb_string.h
//  YinYang
//
//  Created by Андрей Куницын on 5/13/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_string_h
#define YinYang_sb_string_h

#include <string>
#include <functional>

namespace sb {
    using std::string;
    
    struct string_cmp : public std::binary_function<const char*, const char*, bool> {
        bool operator()( const char* lhs, const char* rhs ) const {
            return strcmp(lhs,rhs) < 0;
        }
    };
}


#endif
