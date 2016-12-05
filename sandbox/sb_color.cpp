//
//  sb_color.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 5/20/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_color.h"
#include <cstring>

namespace Sandbox {
    
    static const char hex_chars[] = "0123456789abcdef";
    
    static inline int conv_hex_char(int c) {
        if ( (c>='0')&&(c<='9')) return c-'0';
        if ( (c>='a')&&(c<='f')) return (c-'a')+0xa;
        if ( (c>='A')&&(c<='F')) return (c-'A')+0xa;
        return 0;
    }
    
    static inline GHL::Byte conv_hex(const char* str) {
        return static_cast<GHL::Byte>((conv_hex_char(str[0])<<4) | conv_hex_char(str[1]));
    }
    
    Color Color::FromString( const char* str ) {
        if (!str) return Color(0,0,0,1);
        if (*str=='#')
            ++str;
        size_t len = ::strlen(str);
        GHL::Byte c[4];
        if (len>=6) {
            c[0]=conv_hex(str);
            c[1]=conv_hex(str+2);
            c[2]=conv_hex(str+4);
            if (len>=8) {
                c[3]=conv_hex(str+6);
            } else {
                c[3]=0xff;
            }
        }
        return from_bytes(c);
    }
    
    sb::string Color::ToStringRGB() const {
        char buf[8] = {'#','0','0','0','0','0','0',0};
        GHL::Byte v = conv_float(r);
        buf[1]=hex_chars[v>>4];
        buf[2]=hex_chars[v&0xf];
        v = conv_float(g);
        buf[3]=hex_chars[v>>4];
        buf[4]=hex_chars[v&0xf];
        v = conv_float(b);
        buf[5]=hex_chars[v>>4];
        buf[6]=hex_chars[v&0xf];
        return buf;
    }

    
    const Color& Color::white() {
        static Color _c;
        return _c;
    }
}
