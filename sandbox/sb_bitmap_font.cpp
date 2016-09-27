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
    
    BitmapFont::BitmapFont() : Font(FontDataPtr(new FontData())) {
       
    }
    
    BitmapFont::~BitmapFont() {
        
    }
    
    
    	
	void BitmapFont::AddGlypth(const ImagePtr& img,const char* code,float asc) {
		if (!code) return;
		if (code[0]==0) return;
        UTF32Char ch = 0;
        get_char(code,ch);
        
        FontData::Glypth* glyph = m_data->add_glypth_int(ch);
		
        glyph->img = img ? *img : Image();
		glyph->asc = asc;
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
        FontData::Glypth* g = m_data->get_glypth_int(ch_from);
		if (g) {
			FontData::Kerning k = { ch_to, offset };
			g->kerning.push_back(k);
		}
	}
	
	
}
