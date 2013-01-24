//
//  sb_utf8.cpp
//  YinYang
//
//  Created by Andrey Kunitsyn on 10/23/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "sb_utf.h"
#include <cstring>

namespace Sandbox {
    
    const unsigned char UTF8_BYTE_MASK = 0xBF;
    const unsigned char UTF8_BYTE_MARK = 0x80;
    const unsigned char UTF8_BYTE_MASK_READ = 0x3F;
    const unsigned char UTF8_FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
	
    const char* get_char(const char* s,UTF32Char& ch)
    {
		unsigned int length;
		
		const unsigned char* str = reinterpret_cast<const unsigned char*>(s);
		
		if (*str < UTF8_BYTE_MARK)
		{
			ch = *str;
			return s + 1;
		}
		else if (*str < 0xC0)
		{
			ch = ' ';
			return s + 1;
		}
		else if (*str < 0xE0) length = 2;
		else if (*str < 0xF0) length = 3;
		else if (*str < 0xF8) length = 4;
		else
		{
			ch = ' ';
			return s + 1;
		}
		
		ch = (*str++ & ~UTF8_FIRST_BYTE_MARK[length]);
		
		// Scary scary fall throughs.
		switch (length)
		{
			case 4:
				ch <<= 6;
				ch += (*str++ & UTF8_BYTE_MASK_READ);
			case 3:
				ch <<= 6;
				ch += (*str++ & UTF8_BYTE_MASK_READ);
			case 2:
				ch <<= 6;
				ch += (*str++ & UTF8_BYTE_MASK_READ);
		}
		
		return reinterpret_cast<const char*>(str);
    }
    const char* next_char(const char* s)
    {
		
		const unsigned char* str = reinterpret_cast<const unsigned char*>(s);
		
		if (*str < UTF8_BYTE_MARK)
		{
			return s + 1;
		}
		else if (*str < 0xC0)
		{
			return s + 1;
		}
		else if (*str < 0xE0) return s + 2;
		else if (*str < 0xF0) return s + 3;
		else if (*str < 0xF8) return s + 4;
		
        return s + 1;
		
	}
    
    static UTF8Char* wlow(UTF8Char* result, UTF32Char ch)
    {
        // U+0000..U+007F
        if (ch < 0x80)
        {
            *result = static_cast<UTF8Char>(ch);
            return result + 1;
        }
        // U+0080..U+07FF
        else if (ch < 0x800)
        {
            result[0] = static_cast<UTF8Char>(0xC0 | (ch >> 6));
            result[1] = static_cast<UTF8Char>(0x80 | (ch & 0x3F));
            return result + 2;
        }
        // U+0800..U+FFFF
        else
        {
            result[0] = static_cast<UTF8Char>(0xE0 | (ch >> 12));
            result[1] = static_cast<UTF8Char>(0x80 | ((ch >> 6) & 0x3F));
            result[2] = static_cast<UTF8Char>(0x80 | (ch & 0x3F));
            return result + 3;
        }
    }
    
    static UTF8Char* whigh(UTF8Char* result, UTF32Char ch)
    {
        // U+10000..U+10FFFF
        result[0] = static_cast<UTF8Char>(0xF0 | (ch >> 18));
        result[1] = static_cast<UTF8Char>(0x80 | ((ch >> 12) & 0x3F));
        result[2] = static_cast<UTF8Char>(0x80 | ((ch >> 6) & 0x3F));
        result[3] = static_cast<UTF8Char>(0x80 | (ch & 0x3F));
        return result + 4;
    }
    
    static UTF8Char* wany(UTF8Char* result, UTF32Char ch)
    {
        return (ch < 0x10000) ? wlow(result, ch) : whigh(result, ch);
    }
    
    void append_char( sb::string& s, UTF32Char ch ) {
        char buf[8] = {0};
        *(wany(reinterpret_cast<UTF8Char*>(buf), ch)) = 0;
        s.append(buf);
    }
    
    sb::string strip( const sb::string& src, const sb::string& chars ) {
        UTF32Char ch = 0;
        sb::string res;
        const char* src_it = src.c_str();
        while (*src_it) {
            src_it = get_char(src_it, ch);
            const char* chars_it = chars.c_str();
            bool found = false;
            while (*chars_it) {
                UTF32Char ch1;
                chars_it = get_char(chars_it, ch1);
                if (ch1==ch) { found = true; break; }
            }
            if (found) {
                continue;
            }
            append_char(res,ch);
        }
        return res;
    }
    
    sb::string toupper( const sb::string& str ) {
        const char* src_it = str.c_str();
        sb::string res;
        while (*src_it) {
            UTF32Char ch;
            src_it = get_char(src_it, ch);
            append_char(res, towupper(ch));
        }
        return res;
    }
	
    sb::string::size_type find( const sb::string& instr, const char* str) {
        size_t len = strlen(str);
        const char* src_it = instr.c_str();
        while (*src_it) {
            if (strncmp(src_it, str, len)==0)
                return src_it - instr.c_str();
            src_it = next_char(src_it);
        }
        return sb::string::npos;
    }
    sb::string::size_type find( const sb::string& instr, const char* str,sb::string::size_type from) {
        size_t len = strlen(str);
        if (from>instr.length())
            from = instr.length();
        const char* src_it = instr.c_str() + from;
        while (*src_it) {
            if (strncmp(src_it, str, len)==0)
                return src_it - instr.c_str();
            src_it = next_char(src_it);
        }
        return sb::string::npos;
    }
    
}
