//
//  sb_bitmap_font.cpp
//  YinYang
//
//  Created by Andrey Kunitsyn on 10/23/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "sb_bitmap_font.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::BitmapFont,Sandbox::Font)

namespace Sandbox {
    
    BitmapFont::BitmapFont() {
       
    }
    
    BitmapFont::~BitmapFont() {
        
    }
    
    
    
    void BitmapFont::FixupChars(const char* from, const char* to) {
        while (from && *from) {
            UTF32Char chFrom = 0;
            UTF32Char chTo = 0;
            from = get_char(from, chFrom);
            to = get_char(to, chTo);
            Glypth* glyph = get_glypth_int(chFrom);
            if (!glyph) {
                glyph = add_glypth_int(chFrom);
            }
            const Glypth* toGlyph = get_glypth(chTo);
            if (toGlyph) {
                *glyph = *toGlyph;
                glyph->code = chFrom;
            }
        }
    }
	
	void BitmapFont::AddGlypth(const ImagePtr& img,const char* code,float asc) {
		if (!code) return;
		if (code[0]==0) return;
        UTF32Char ch = 0;
        get_char(code,ch);
        
        Glypth* glyph = add_glypth_int(ch);
		
        glyph->img = img ? *img : Image();
		glyph->asc = asc;
		glyph->code = ch;
    }
	void BitmapFont::AddKerningPair(const char* from,const char* to,float offset) {
		if (!from) return;
		if (from[0]==0) return;
		if (!to) return;
		if (to[0]==0) return;
		UTF32Char ch_from = 0;
		UTF32Char ch_to = 0;
		get_char(from,ch_from);
		get_char(to,ch_to);
		Glypth* g = get_glypth_int(ch_from);
		if (g) {
			Kerning k = { ch_to, offset };
			g->kerning.push_back(k);
		}
	}
	
	
}