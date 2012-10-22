//
//  sb_utf8.cpp
//  YinYang
//
//  Created by Andrey Kunitsyn on 10/23/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "sb_utf.h"

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
	
    /*static const char* next_char(const char* s) {
     unsigned int length;
     
     const unsigned char* str = reinterpret_cast<const unsigned char*>(s);
     
     if (*str < UTF8_BYTE_MARK)
     {
     return s + 1;
     }
     else if (*str < 0xC0)
     {
     return s + 1;
     }
     else if (*str < 0xE0) length = 2;
     else if (*str < 0xF0) length = 3;
     else if (*str < 0xF8) length = 4;
     else
     {
     return s + 1;
     }
     return reinterpret_cast<const char*>(str+length);
     }*/

    
}
