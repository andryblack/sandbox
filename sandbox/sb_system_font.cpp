#include "sb_system_font.h"
#include "sb_application.h"
#include "sb_resources.h"

SB_META_DECLARE_OBJECT(Sandbox::SystemFont,Sandbox::OutlineFontDataProvider)

namespace Sandbox {
    
    sb::intrusive_ptr<SystemFont> SystemFont::Load( Application* app, const char* name, const SystemFontConfig& config ) {
        GHL::System* sys = app->GetSystem();
        if (!sys) {
            return SystemFontPtr();
        }
        GHL::FontConfig fc;
        fc.name = name;
        fc.xscale = config.x_scale;
        float scale = app->GetResources()->GetScale() * config.scale;
        fc.outline_width = 0.0f;
        fc.size = config.size * scale;
        
        GHL::Font* fnt = sys->CreateFont(&fc);
        if (!fnt) {
            return SystemFontPtr();
        }
        SystemFont* res = new SystemFont(fnt,app->GetResources()->GetDefaultTexturePool());
        res->m_config = config;
        res->m_scale = scale;
        res->FillMetrics();
        if (config.outline) {
            fc.outline_width = (config.outline_width * scale);
            res->set_outline_font(sys->CreateFont(&fc));
        }
        return SystemFontPtr(res);
    }
    
    SystemFont::SystemFont(GHL::Font* fnt,const TexturePoolPtr& pool) : OutlineFontDataProvider(),
        m_font(fnt),m_outline_font(0), m_textures(pool) {
        
    }
    
    void SystemFont::set_outline_font(GHL::Font* fnt) {
        m_outline_font = fnt;
    }
    
    SystemFont::~SystemFont() {
        m_font->Release();
        if (m_outline_font) {
            m_outline_font->Release();
        }
    }
    
    void SystemFont::FillMetrics() {
        set_size(m_config.size);
        set_height( (m_font->GetAscender()-m_font->GetDescender()) / m_scale );
        set_baseline(-m_font->GetDescender() / m_scale );
    }
    
    bool SystemFont::preallocate_symb(UTF32Char ch) {
        GHL::Glyph g;
        g.bitmap = 0;
        bool res = false;
        FontData::Glypth* main_glyph = 0;
        if (m_font->RenderGlyph(ch, &g)) {
            main_glyph = m_data->add_glypth_int(ch);
            main_glyph->asc = g.advance;
            
            if (g.bitmap) {
                GHL::Texture* ntex = 0;
                int bmx = 0;
                int bmy = 0;
                TexturePtr tex = m_textures->Alloc(g.bitmap->GetWidth()+1,g.bitmap->GetHeight()+1,
                                                   bmx,bmy,ntex,
                                                   GHL_ImageFormatToTextureFormat(g.bitmap->GetFormat()));
                ntex->SetData(bmx, bmy, g.bitmap);
                
                float iscale = tex->GetScale();
                main_glyph->asc/=m_scale;
                
                main_glyph->img = Image(tex,iscale*bmx ,iscale*bmy,iscale*g.bitmap->GetWidth(),iscale*g.bitmap->GetHeight());
                main_glyph->img.SetHotspot(Sandbox::Vector2f(g.x * iscale,g.y * iscale));
                main_glyph->img.SetSize(float(g.bitmap->GetWidth()) / m_scale , float(g.bitmap->GetHeight()) / m_scale );
            
                res = true;
            }
            
        }
        if (g.bitmap) {
            g.bitmap->Release();
            g.bitmap = 0;
        }
        if (res && m_outline_font) {
            FontData::Glypth* gl = m_outline_data->add_glypth_int(ch);
            if (m_outline_font->RenderGlyph(ch, &g)) {
                gl->asc = g.advance;
                
                if (g.bitmap) {
                    GHL::Texture* ntex = 0;
                    int bmx = 0;
                    int bmy = 0;
                    TexturePtr tex = m_textures->Alloc(g.bitmap->GetWidth()+1,g.bitmap->GetHeight()+1,
                                                       bmx,bmy,ntex,
                                                       GHL_ImageFormatToTextureFormat(g.bitmap->GetFormat()));
                    ntex->SetData(bmx, bmy, g.bitmap);
                    
                    float iscale = tex->GetScale();
                    gl->asc/=m_scale;
                    
                    gl->img = Image(tex,iscale*bmx ,iscale*bmy,iscale*g.bitmap->GetWidth(),iscale*g.bitmap->GetHeight());
                    gl->img.SetHotspot(Sandbox::Vector2f(g.x * iscale,g.y * iscale));
                    gl->img.SetSize(float(g.bitmap->GetWidth()) / m_scale , float(g.bitmap->GetHeight()) / m_scale );
                    
                }
            } else {
                gl->img = main_glyph->img;
                gl->asc = main_glyph->asc;
            }
            if (g.bitmap) {
                g.bitmap->Release();
            }
        }
        return res;
    }
        
    void SystemFont::AllocateSymbols( const char* text ) {
        FontDataProvider::AllocateSymbols(text);
        m_textures->Commit();
    }
}
