/*
 *  sb_free_type_font.h
 *
 */

#ifndef SB_FREE_TYPE_FONT_H
#define SB_FREE_TYPE_FONT_H

#include "sb_font.h"

namespace Sandbox {
    
    class Resources;
    
    struct FreeTypeFontConfig {
        float   size;
        bool    outline;
        float   dpi;
        float   scale;
        float   x_scale;
        GHL::UInt32 substitute_code;
    };
    
    class FreeTypeFontChild : public Font {
        SB_META_OBJECT;
    public:
        explicit FreeTypeFontChild( const FontPtr& parent, const FontDataPtr& data );
        virtual void AllocateSymbols( const char* text );
        virtual bool HasGlyph(GHL::UInt32 code) const {
            return m_parent ? m_parent->HasGlyph(code) : Font::HasGlyph(code);
        }
    private:
        FontPtr m_parent;
    };
    typedef sb::intrusive_ptr<FreeTypeFontChild> FreeTypeFontChildPtr;
    
    class FreeTypeFont : public Font {
        SB_META_OBJECT
    public:
        static sb::intrusive_ptr<FreeTypeFont> Load( Resources* resources, const char* file, const FreeTypeFontConfig& config );
        virtual ~FreeTypeFont();
        virtual void AllocateSymbols( const char* text );
        
        const FontDataPtr& GetMainData() const { return m_data; }
        const FontDataPtr& GetOutlineData() const { return m_outline_data; }
        
        FreeTypeFontChildPtr CreateMainChild();
        FreeTypeFontChildPtr CreateOutlineChild();
        
        void SetCharImage(GHL::UInt32 code,const Sandbox::ImagePtr& image,float advance);
        void AddCharImage(GHL::UInt32 code,const Sandbox::ImagePtr& image,float advance);
        virtual bool HasGlyph(GHL::UInt32 code) const;
    private:
        struct Impl;
        Impl*   m_impl;
        explicit FreeTypeFont(Impl* impl);
        void preallocate_text(const char* text);
        bool preallocate_symb(GHL::UInt32 s);
        FontDataPtr m_outline_data;
    };
    typedef sb::intrusive_ptr<FreeTypeFont> FreeTypeFontPtr;
    
}

#endif /*SB_FONT_H*/
