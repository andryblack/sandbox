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
        float   outline_width;
        GHL::UInt32 substitute_code;
    };
    
    
    
    class FreeTypeFont : public OutlineFontDataProvider {
        SB_META_OBJECT
    public:
        static sb::intrusive_ptr<FreeTypeFont> Load( Resources* resources, const char* file, const FreeTypeFontConfig& config );
        static void Release();
        virtual ~FreeTypeFont();
        virtual void AllocateSymbols( const char* text ) SB_OVERRIDE;
        
        
    private:
        struct Impl;
        Impl*   m_impl;
        explicit FreeTypeFont(Impl* impl);
        virtual bool preallocate_symb( UTF32Char s) SB_OVERRIDE;
    };
    typedef sb::intrusive_ptr<FreeTypeFont> FreeTypeFontPtr;
    
}

#endif /*SB_FREE_TYPE_FONT_H*/
