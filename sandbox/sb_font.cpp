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
#include "sb_text_data.h"

SB_META_DECLARE_OBJECT(Sandbox::Font,Sandbox::meta::object)
SB_META_DECLARE_OBJECT(Sandbox::FontPass,Sandbox::meta::object)
SB_META_DECLARE_OBJECT(Sandbox::FontDrawAttributes,Sandbox::DrawAttributes)

namespace Sandbox {
    
    
    
    FontPass::FontPass( const FontDataPtr& data, const sb::string& name) : m_data(data),m_name(name) {
        m_use_color = false;
    }
	
    float FontPass::DrawI( Graphics& g, DrawAttributes* attributes,const Sandbox::Vector2f& _pos , const char* text) const {
        const FontData::Glypth* prev = 0;
        FontDrawAttributes* font_draw = 0;
        if (attributes) {
            font_draw = meta::sb_dynamic_cast<FontDrawAttributes>(attributes);
        }
        Sandbox::Color c = g.GetColor();
        if (font_draw) {
            if (!font_draw->BeginPass(g,*this))
                return 0.0;
        } else if (!m_use_color) {
            g.SetColor(c * m_color);
        }
        
        Sandbox::Vector2f pos = _pos + m_offset;
        while (*text) {
            UTF32Char ch = 0;
            text = get_char(text,ch);
            if (ch) {
                const FontData::Glypth* gl = m_data->get_glypth(ch);
                if (gl) {
                    pos.x+=FontData::getKerning(prev,ch);
                    if (gl->img.GetTexture()) g.DrawImage(gl->img, attributes, pos);
                    pos.x+=gl->asc;
                }
                prev=gl;
            }
        }
        if (font_draw) {
            font_draw->EndPass(g,*this);
        }
        g.SetColor(c);
        return pos.x - _pos.x;
    }
    void FontPass::DrawI( Graphics& g, DrawAttributes* attributes,
                     const TextData& text) const {
        Sandbox::Color c = g.GetColor();
        FontDrawAttributes* font_draw = 0;
        if (attributes) {
            font_draw = meta::sb_dynamic_cast<FontDrawAttributes>(attributes);
        }
        if (font_draw) {
            if (!font_draw->BeginPass(g,*this))
                return;
        } else if (!m_use_color) {
             g.SetColor(c * m_color);
        }
        for (TextData::LinesData::const_iterator line = text.data.begin();line!=text.data.end();++line) {
            Sandbox::Vector2f pos = line->offset + m_offset;
            for (TextData::SymbolsData::const_iterator symbol = line->data.begin();symbol!=line->data.end();++symbol) {
                if (symbol->type == TextData::SYMBOL_SET_COLOR) {
                    if (m_use_color) {
                        g.SetColor(c * Color(symbol->data.color));
                    }
                } else {
                    const FontData::Glypth* gl = m_data->get_glypth(symbol->data.symbol.code);
                    if (gl && gl->img.GetTexture()) {
                        g.DrawImage(gl->img, attributes, pos);
                    }
                    pos.x+=symbol->data.symbol.offset;
                }
            }
        }
        if (font_draw) {
            font_draw->EndPass(g,*this);
        }
        g.SetColor(c);
    }
    
    void FontPass::DrawCroppedI( Graphics& g, DrawAttributes* attributes,
                        const Rectf& rect,
                         const TextData& text) const {
        Sandbox::Color c = g.GetColor();
        FontDrawAttributes* font_draw = 0;
        if (attributes) {
            font_draw = meta::sb_dynamic_cast<FontDrawAttributes>(attributes);
        }
        if (font_draw) {
            if (!font_draw->BeginPass(g,*this))
                return;
        } else if (!m_use_color) {
            g.SetColor(c * m_color);
        }
        for (TextData::LinesData::const_iterator line = text.data.begin();line!=text.data.end();++line) {
            Sandbox::Vector2f pos = line->offset + m_offset;
            for (TextData::SymbolsData::const_iterator symbol = line->data.begin();symbol!=line->data.end();++symbol) {
                if (symbol->type == TextData::SYMBOL_SET_COLOR) {
                    if (m_use_color) {
                        g.SetColor(c * Color(symbol->data.color));
                    }
                } else {
                    const FontData::Glypth* gl = m_data->get_glypth(symbol->data.symbol.code);
                    if (gl && gl->img.GetTexture()) {
                        g.DrawImage(gl->img, attributes, pos, rect);
                    }
                    pos.x+=symbol->data.symbol.offset;
                }
            }
        }
        if (font_draw) {
            font_draw->EndPass(g,*this);
        }
        g.SetColor(c);
    }

		
    Font::Font(const FontDataPtr& main_data) : m_data(main_data) {
        
        m_size = 8.0f;
        m_height = 16.0f;
        m_baseline = 0.0f;
        m_xheight = m_height;
	}
	
	Font::~Font() {
	
	}
    
    void Font::ClearPasses() {
        m_passes.clear();
    }
    void Font::AddPass( const FontPassPtr& pass ) {
        sb_assert(pass);
        m_passes.push_back(pass);
    }
    
    
    
    float Font::GetTextWidth(const char* text) const {
        if (!text) return 0.0f;
        const_cast<Font*>(this)->AllocateSymbols(text);
        return m_data->GetTextWidthI( text );
    }
    
    bool Font::MovePosition(Vector2f& pos,UTF32Char prev,UTF32Char next) const {
        if (next) {
            const FontData::Glypth* gl = m_data->get_glypth(next);
            if (gl) {
                const FontData::Glypth* glprev = m_data->get_glypth(prev);
                pos.x+=gl->asc;
                pos.x+=FontData::getKerning(glprev,next);
            }
        }
        return true;
    }
    
    
    float Font::Draw(Graphics& g, DrawAttributes* attributes, const Vector2f& _pos,const char* text,FontAlign align) const {
        if (!text) return 0;
        const_cast<Font*>(this)->AllocateSymbols(text);
        Sandbox::Vector2f pos = m_data->AlignI(_pos,align,text);
        float res = 0.0f;
        for (FontPassList::const_iterator it = m_passes.begin();it!=m_passes.end();++it) {
            res = (*it)->DrawI(g, attributes, pos, text);
        }
        return res;
    }
    void Font::Draw(Graphics& g,
                    DrawAttributes* attributes,const TextData& data) const {
        for (FontPassList::const_iterator it = m_passes.begin();it!=m_passes.end();++it) {
            (*it)->DrawI(g, attributes, data);
        }
    }
    void Font::DrawCropped(Graphics& g,
                    DrawAttributes* attributes,
                           const Rectf& rect,
                           const TextData& data) const {
        for (FontPassList::const_iterator it = m_passes.begin();it!=m_passes.end();++it) {
            (*it)->DrawCroppedI(g, attributes, rect,  data);
        }
    }

}
