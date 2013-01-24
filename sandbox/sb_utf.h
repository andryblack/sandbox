//
//  sb_utf8.h
//  YinYang
//
//  Created by Andrey Kunitsyn on 10/23/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_utf8_h
#define YinYang_sb_utf8_h

#include <ghl_types.h>
#include <sb_string.h>

namespace Sandbox {
    
    typedef GHL::Byte UTF8Char;
    typedef GHL::UInt16 UTF16Char;
    typedef GHL::UInt32 UTF32Char;
    const char* get_char(const char* s,UTF32Char& ch);
    void append_char( sb::string& s, UTF32Char ch );
    sb::string strip( const sb::string& src, const sb::string& chars );
    sb::string toupper( const sb::string& str );
    sb::string::size_type find( const sb::string& instr, const char* str);
    sb::string::size_type find( const sb::string& instr, const char* str,sb::string::size_type from);
}

#endif
