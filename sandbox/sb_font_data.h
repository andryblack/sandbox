#ifndef SB_FONT_DATA_H_INCLUDED
#define SB_FONT_DATA_H_INCLUDED

#include "meta/sb_meta.h"
#include "sb_image.h"
#include <sbstd/sb_vector.h>
#include <sbstd/sb_intrusive_ptr.h>
#include <sbstd/sb_map.h>
#include "sb_utf.h"

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
            UTF32Char code;
            float	offset;
        };
        struct Glypth {
            Image   img;
            float	asc;
            sb::vector<Kerning> kerning;
            inline float bearingX() const {
                return -img.GetHotspotReal().x;
            }
            inline float bearingY() const {
                return -img.GetHotspotReal().y;
            }
            inline float width() const {
                return img.GetWidth();
            }
            inline float height() const {
                return img.GetHeight();
            }
        };
        
        bool has_glypth(UTF32Char code) const;
        const Glypth* get_glypth(UTF32Char code) const;
        Glypth* get_glypth_int(UTF32Char code);
        Glypth* add_glypth_int(UTF32Char code);
        static float getKerning(const Glypth* g,UTF32Char to);
        void reserve(size_t size);
        
        Sandbox::Vector2f AlignI( const Sandbox::Vector2f& pos ,
                                        FontAlign align,
                                        const char* text ) const;
        float GetTextWidthI(const char* text) const;
        
        void FixupChars(const char* from, const char* to);
        void SetSubsituteCode(UTF32Char code);
    private:
        typedef sb::map<UTF32Char,Glypth> GlyphMap;
        GlyphMap m_glypths;
        UTF32Char m_substitute_code;
        Glypth m_substitute_glyph;
    };
    typedef sb::intrusive_ptr<FontData> FontDataPtr;
    
    class FontDataProvider : public meta::object{
        SB_META_OBJECT
    private:
        float   m_size;
        float   m_height;
        float   m_xheight;
        float   m_baseline;
    protected:
        FontDataPtr m_data;
        void    set_height(float height) { m_height = height; }
        void    set_size(float size) { m_size = size; }
        void    set_baseline(float baseline) { m_baseline = baseline; }
        void    set_x_height(float h) {m_xheight = h;}
        explicit FontDataProvider();
    public:
        virtual void AllocateSymbols( const char* text );
        const FontDataPtr& GetMainData() const { return m_data; }
        float GetSize() const { return m_size; }
        float GetHeight() const { return m_height; }
        float GetBaseline() const { return m_baseline; }
        float GetXHeight() const { return m_xheight; }
        bool HasGlyph(UTF32Char ch);
        const FontData::Glypth* GetGlyph(UTF32Char ch) const;
        void AddCharImage(UTF32Char code,const Sandbox::ImagePtr& image,float advance);
        virtual void SetCharImage(UTF32Char code,const Sandbox::ImagePtr& image,float advance);
        
        // internal
        void set_data(const FontDataPtr& data);
        virtual bool preallocate_symb(UTF32Char ch) { return false; }
    };
    typedef sb::intrusive_ptr<FontDataProvider> FontDataProviderPtr;
    
    
    class OutlineFontDataProvider : public FontDataProvider {
        SB_META_OBJECT
    protected:
        FontDataPtr m_outline_data;
        explicit OutlineFontDataProvider();
    public:
        const FontDataPtr& GetOutlineData() const { return m_outline_data; }
        virtual void SetCharImage(UTF32Char code,const Sandbox::ImagePtr& image,float advance) SB_OVERRIDE;
        void set_outline_data(const FontDataPtr& data);
    };
}

#endif /*SB_FONT_DATA_H_INCLUDED*/
