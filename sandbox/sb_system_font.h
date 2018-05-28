/*
 *  sb_system_font.h
 *
 */

#ifndef SB_SYSTEM_FONT_H
#define SB_SYSTEM_FONT_H

#include "sb_font_data.h"
#include <ghl_font.h>
#include "sb_texture_pool.h"

namespace Sandbox {
    class Application;
    
    struct SystemFontConfig {
        float size;
        float scale;
        float x_scale;
        bool outline;
        float outline_width;
    };
    class SystemFont : public OutlineFontDataProvider {
        SB_META_OBJECT
    private:
        GHL::Font* m_font;
        GHL::Font* m_outline_font;
        SystemFontConfig m_config;
        float m_scale;
    public:
        static sb::intrusive_ptr<SystemFont> Load( Application* resources, const char* name, const SystemFontConfig& config );
        
        virtual ~SystemFont();
        
        virtual void AllocateSymbols( const char* text ) SB_OVERRIDE;
    
    private:
        explicit SystemFont(GHL::Font* impl,const TexturePoolPtr& pool);
        TexturePoolPtr m_textures;
        void set_outline_font(GHL::Font* fnt);
        bool preallocate_symb(UTF32Char ch) SB_OVERRIDE;
        void FillMetrics();
    };
    typedef sb::intrusive_ptr<SystemFont> SystemFontPtr;
    
}

#endif /*SB_FREE_TYPE_FONT_H*/
