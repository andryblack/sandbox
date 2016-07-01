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

SB_META_DECLARE_OBJECT(Sandbox::Font,Sandbox::meta::object)

namespace Sandbox {
	
	
		
	Font::Font() {
        m_glypths.reserve(12);
        
        m_size = 8.0f;
        m_height = 16.0f;
        m_baseline = 0.0f;
        m_xheight = m_height;
	}
	
	Font::~Font() {
	
	}
	
    void Font::Reserve(size_t size) {
        m_glypths.reserve(size);
    }
    
    
    const Font::Glypth* Font::get_glypth(GHL::UInt32 code) const {
        for (std::vector<Glypth>::const_iterator it=m_glypths.begin();it!=m_glypths.end();it++) {
            if (it->code == code) return &(*it);
        }
        return 0;
    }
    Font::Glypth* Font::get_glypth_int(GHL::UInt32 code) {
        for (std::vector<Glypth>::iterator it=m_glypths.begin();it!=m_glypths.end();it++) {
            if (it->code == code) return &(*it);
        }
        return 0;
    }
    Font::Glypth* Font::add_glypth_int(GHL::UInt32 code) {
        m_glypths.push_back(Glypth());
        m_glypths.back().code = code;
        return &m_glypths.back();
    }
    float Font::getKerning(const Glypth* g,GHL::UInt32 to) const {
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
    
    bool Font::MovePosition(Vector2f& pos,UTF32Char prev,UTF32Char next) const {
        if (next) {
            const Glypth* gl = get_glypth(next);
            if (gl) {
                const Glypth* glprev = get_glypth(prev);
                pos.x+=gl->asc;
                pos.x+=getKerning(glprev,next);
            }
        }
        return true;
    }
    
    float Font::Draw(Graphics& g,const DrawAttributesPtr& attributes, const Vector2f& _pos,const char* text,FontAlign align) const {
        if (!text) return 0;
        Vector2f pos = _pos;
        float align_x = 0.0f;
        if (align == ALIGN_RIGHT) {
            align_x=GetTextWidth(text);
        } else if (align==ALIGN_CENTER) {
            align_x=int(GetTextWidth(text)*0.5f);
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
                    if (gl->img.GetTexture()) g.DrawImage(gl->img, attributes, pos);
                    pos.x+=gl->asc;
                }
                prev=gl;
            }
        }
        return pos.x - _pos.x + align_x;
    }

}
