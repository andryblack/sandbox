/*
 *  sb_font.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 12.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_font.h"
#include "sb_graphics.h"

namespace Sandbox {
	
	typedef GHL::UInt16 UTF16Char;

	const unsigned char UTF8_BYTE_MASK = 0xBF;
    const unsigned char UTF8_BYTE_MARK = 0x80;
    const unsigned char UTF8_BYTE_MASK_READ = 0x3F;
    const unsigned char UTF8_FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
	
    static const char* get_char(const char* s,UTF16Char& ch)
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
	
	Font::Font() {
		m_glypths.reserve(12);
	}
	
	Font::~Font() {
	
	}
	
	void Font::Reserve(size_t size) {
		m_glypths.reserve(size);
	}
	
	void Font::AddGlypth(const ImagePtr& img,const char* code,float asc) {
		if (!code) return;
		if (code[0]==0) return;
		m_glypths.resize(m_glypths.size()+1);
		UTF16Char ch = 0;
		get_char(code,ch);
		m_glypths.back().img = img;
		m_glypths.back().asc = asc;
		m_glypths.back().code = ch;
	}
	void Font::AddKerningPair(const char* from,const char* to,float offset) {
		if (!from) return;
		if (from[0]==0) return;
		if (!to) return;
		if (to[0]==0) return;
		UTF16Char ch_from = 0;
		UTF16Char ch_to = 0;
		get_char(from,ch_from);
		get_char(to,ch_to);
		Glypth* g = const_cast<Glypth*>(get_glypth(ch_from));
		if (g) {
			Kerning k = { ch_to, offset };
			g->kerning.push_back(k);
		}
	}
	const Font::Glypth* Font::get_glypth(GHL::UInt16 code) const {
		for (std::vector<Glypth>::const_iterator it=m_glypths.begin();it!=m_glypths.end();it++) {
			if (it->code == code) return &(*it);
		}
		return 0;
	}
	float Font::getKerning(const Glypth* g,GHL::UInt16 to) const {
		if (g) {
			for (size_t i=0;i<g->kerning.size();i++) {
				if (g->kerning[i].code==to) return g->kerning[i].offset;
			}
		}
		return 0.0f;
	}
	float Font::GetTextWidth(const char* text) const {
		if (!text) return 0.0f;
		float w = 0;
		const Glypth* prev = 0;
		while (*text) {
			UTF16Char ch = 0;
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
	
	void Font::Draw(Graphics& g,const Vector2f& _pos,const char* text,FontAlign align) const {
		if (!text) return;
		Vector2f pos = _pos;
		if (align == ALIGN_RIGHT) {
			pos.x-=GetTextWidth(text);
		} else if (align==ALIGN_CENTER) {
			pos.x-=GetTextWidth(text)*0.5f;
		}
		const Glypth* prev = 0;
		while (*text) {
			UTF16Char ch = 0;
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
	}
	
}
