#include "sb_font_data.h"
#include "sb_graphics.h"
#include "sb_utf.h"

SB_META_DECLARE_OBJECT(Sandbox::FontData,Sandbox::meta::object)

namespace Sandbox {
    
    bool FontData::has_glypth(GHL::UInt32 code) const {
        GlyphMap::const_iterator it = m_glypths.find(code);
        return it != m_glypths.end();
    }
    const FontData::Glypth* FontData::get_glypth(GHL::UInt32 code) const {
        GlyphMap::const_iterator it = m_glypths.find(code);
        if (it==m_glypths.end()) {
            return &m_substitute_glyph;
        }
        return &it->second;
    }
    FontData::Glypth* FontData::get_glypth_int(GHL::UInt32 code) {
        GlyphMap::iterator it = m_glypths.find(code);
        if (it!=m_glypths.end()) {
            return &it->second;
        }
        return 0;
    }
    FontData::Glypth* FontData::add_glypth_int(GHL::UInt32 code) {
        std::pair<GlyphMap::iterator,bool> res = m_glypths.insert(std::make_pair(code, Glypth()));
        return &(res.first->second);
    }
    float FontData::getKerning(const Glypth* g,GHL::UInt32 to) {
        if (g) {
            for (size_t i=0;i<g->kerning.size();i++) {
                if (g->kerning[i].code==to) return g->kerning[i].offset;
            }
        }
        return 0.0f;
    }
    
    void FontData::SetSubsituteCode(GHL::UInt32 code) {
        m_substitute_code = code;
        Glypth* g = get_glypth_int(code);
        if (g) {
            m_substitute_glyph = *g;
        }
    }
    
    void FontData::reserve(size_t size) {
        //m_glypths.reserve(size);
    }

    
    Sandbox::Vector2f FontData::AlignI( const Sandbox::Vector2f& _pos ,
                                   FontAlign align,
                                   const char* text ) const {
        Vector2f pos = _pos;
        float align_x = 0.0f;
        if (align == ALIGN_RIGHT) {
            align_x=GetTextWidthI(text);
        } else if (align==ALIGN_CENTER) {
            align_x=int(GetTextWidthI(text)*0.5f);
        }
        pos.x-=align_x;
        return pos;
    }
    
    float FontData::GetTextWidthI(const char* text) const {
        float w = 0;
        const FontData::Glypth* prev = 0;
        while (*text) {
            UTF32Char ch = 0;
            text = get_char(text,ch);
            if (ch) {
                const FontData::Glypth* gl = get_glypth(ch);
                if (gl) {
                    if (!prev) {
                        if (gl->bearingX() < 0)
                            w += -gl->bearingX();
                    }
                    w+=gl->asc;
                    w+=FontData::getKerning(prev,ch);
                    prev = gl;
                }
            }
        }
        return w;
    }
    
    void FontData::FixupChars(const char* from, const char* to) {
        while (from && *from) {
            UTF32Char chFrom = 0;
            UTF32Char chTo = 0;
            from = get_char(from, chFrom);
            to = get_char(to, chTo);
            FontData::Glypth* glyph = get_glypth_int(chFrom);
            if (!glyph) {
                glyph = add_glypth_int(chFrom);
            }
            const FontData::Glypth* toGlyph = get_glypth(chTo);
            if (toGlyph) {
                *glyph = *toGlyph;
            }
        }
    }
    
}
