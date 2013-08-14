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
        m_glypths.reserve(12);
    }
    
    BitmapFont::~BitmapFont() {
        
    }
    
    void BitmapFont::Reserve(size_t size) {
		m_glypths.reserve(size);
	}
	
	void BitmapFont::AddGlypth(const ImagePtr& img,const char* code,float asc) {
		if (!code) return;
		if (code[0]==0) return;
		m_glypths.resize(m_glypths.size()+1);
		UTF32Char ch = 0;
		get_char(code,ch);
		m_glypths.back().img = img;
		m_glypths.back().asc = asc;
		m_glypths.back().code = ch;
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
		Glypth* g = const_cast<Glypth*>(get_glypth(ch_from));
		if (g) {
			Kerning k = { ch_to, offset };
			g->kerning.push_back(k);
		}
	}
	const BitmapFont::Glypth* BitmapFont::get_glypth(GHL::UInt32 code) const {
		for (std::vector<Glypth>::const_iterator it=m_glypths.begin();it!=m_glypths.end();it++) {
			if (it->code == code) return &(*it);
		}
		return 0;
	}
	float BitmapFont::getKerning(const Glypth* g,GHL::UInt32 to) const {
		if (g) {
			for (size_t i=0;i<g->kerning.size();i++) {
				if (g->kerning[i].code==to) return g->kerning[i].offset;
			}
		}
		return 0.0f;
	}
	float BitmapFont::GetTextWidth(const char* text) const {
		if (!text) return 0.0f;
		float w = 0;
		const Glypth* prev = 0;
		while (*text) {
			UTF32Char ch = 0;
			text = get_char(text,ch);
			if (ch) {
				const Glypth* gl = get_glypth(ch);
				if (gl) {
					w+=gl->asc;
					w+=getKerning(prev,ch);
				}
				prev = gl;
			}
		}
		return w;
	}
	
	float BitmapFont::Draw(Graphics& g,const Vector2f& _pos,const char* text,FontAlign align) const {
		if (!text) return 0;
		Vector2f pos = _pos;
        float align_x = 0.0f;
		if (align == ALIGN_RIGHT) {
			align_x=GetTextWidth(text);
		} else if (align==ALIGN_CENTER) {
			align_x=GetTextWidth(text)*0.5f;
		}
        pos.x-=align_x;
		const Glypth* prev = 0;
		while (*text) {
			UTF32Char ch = 0;
			text = get_char(text,ch);
			if (ch) {
				const Glypth* gl = get_glypth(ch);
				if (gl) {
					pos.x+=getKerning(prev,ch);
					g.DrawImage(*(gl->img), pos);
					pos.x+=gl->asc;
				}
				prev=gl;
			}
		}
        return pos.x - _pos.x + align_x;
	}

}