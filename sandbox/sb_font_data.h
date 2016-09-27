#ifndef SB_FONT_DATA_H_INCLUDED
#define SB_FONT_DATA_H_INCLUDED

#include "meta/sb_meta.h"
#include "sb_image.h"
#include <sbstd/sb_vector.h>
#include <sbstd/sb_intrusive_ptr.h>
#include <sbstd/sb_map.h>

namespace Sandbox {

    enum FontAlign {
        ALIGN_LEFT = 0,
        ALIGN_RIGHT = 1,
        ALIGN_CENTER = 3,
    };

    
    class FontData : public meta::object {
      SB_META_OBJECT
    public:
        struct Kerning {
            GHL::UInt32 code;
            float	offset;
        };
        struct Glypth {
            Image   img;
            float	asc;
            sb::vector<Kerning> kerning;
            inline float bearingX() const {
                return -img.GetHotspot().x;
            }
            inline float bearingY() const {
                return -img.GetHotspot().y;
            }
            inline float width() const {
                return img.GetWidth();
            }
            inline float height() const {
                return img.GetHeight();
            }
        };
        
        bool has_glypth(GHL::UInt32 code) const;
        const Glypth* get_glypth(GHL::UInt32 code) const;
        Glypth* get_glypth_int(GHL::UInt32 code);
        Glypth* add_glypth_int(GHL::UInt32 code);
        static float getKerning(const Glypth* g,GHL::UInt32 to);
        void reserve(size_t size);
        
        Sandbox::Vector2f AlignI( const Sandbox::Vector2f& pos ,
                                        FontAlign align,
                                        const char* text ) const;
        float GetTextWidthI(const char* text) const;
        
        void FixupChars(const char* from, const char* to);
        void SetSubsituteCode(GHL::UInt32 code);
    private:
        typedef sb::map<GHL::UInt32,Glypth> GlyphMap;
        GlyphMap m_glypths;
        GHL::UInt32 m_substitute_code;
        Glypth m_substitute_glyph;
    };
    typedef sb::intrusive_ptr<FontData> FontDataPtr;
}

#endif /*SB_FONT_DATA_H_INCLUDED*/
