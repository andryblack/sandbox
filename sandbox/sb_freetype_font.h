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
    };
    
    class FreeTypeFont : public Font {
        SB_META_OBJECT
    public:
        static sb::intrusive_ptr<FreeTypeFont> Load( Resources* resources, const char* file, const FreeTypeFontConfig& config );
        virtual ~FreeTypeFont();
        virtual float Draw(Graphics& g,const Vector2f& pos,const char* text,FontAlign align) const;
        virtual float GetTextWidth(const char* text) const;
    private:
        struct Impl;
        Impl*   m_impl;
        explicit FreeTypeFont(Impl* impl);
        
        void preallocate_text(const char* text);
    };
}

#endif /*SB_FONT_H*/
