#include "sb_texture_pool.h"
#include "sb_resources.h"
#include "sb_log.h"

namespace Sandbox {
    
    static const char* MODULE = "texture-pool";
    
    TexturePool::TexturePool( Resources* resources  ) : m_resources(resources) {
        m_initial_texture_width = 512 * resources->GetScale();
        m_initial_texture_height = 512 * resources->GetScale();
    }
    
    TexturePtr TexturePool::Alloc(int w, int h, int& x, int& y,GHL::Texture* &ntex,GHL::TextureFormat fmt) {
        TextureData* data = 0;
        for (TextureDataVector::iterator it = m_textures.begin();it!=m_textures.end();++it) {
            if (it->fmt != fmt)
                continue;
            int tw = it->tex->GetRealWidth();
            int th = it->tex->GetRealHeight();
            if ((it->pos_x + w) > tw) {
                if ((it->pos_y + it->max_h + h) > th) {
                    continue;
                }
            }
            if ((it->pos_y+h) > th) {
                continue;
            }
            data = &(*it);
            break;
        }
        if (!data) {
            /// allocate new texture
            SB_LOGI("allocate texture " << m_initial_texture_width << "x" << m_initial_texture_height);
            GHL::Image* fill = GHL_CreateImage(m_initial_texture_width,
                                               m_initial_texture_height,
                                               GHL_TextureFormatToImageFormat(fmt));
            fill->Fill(0x00000000);
            TexturePtr tex = m_resources->CreateTexture(fill, 1.0f / m_resources->GetScale(), fmt);
            fill->Release();
            tex->SetFiltered(true);
            TextureData d;
            d.tex = tex;
            d.pos_x = 1;
            d.pos_y = 1;
            d.max_h = 0;
            d.fmt = fmt;
            m_textures.push_back(d);
            data = &m_textures.back();
        }
        int tw = data->tex->GetRealWidth();
        int th = data->tex->GetRealHeight();
        if ((data->pos_x + w) > tw) {
            data->pos_x = 1;
            data->pos_y += data->max_h;
            data->max_h = 0;
        }
        x = data->pos_x;
        y = data->pos_y;
        sb_assert((x+w)<=tw);
        sb_assert((y+h)<=th);
        
        if (h>data->max_h)
            data->max_h = h;
        data->pos_x += w;
        ntex = data->tex->Present(m_resources);
        if (std::find(m_changed_textures.begin(),m_changed_textures.end(),ntex)==m_changed_textures.end()) {
            m_changed_textures.push_back(ntex);
        }
        
        return data->tex;
    }
    
    void TexturePool::Commit() {
        for (sb::vector<GHL::Texture*>::iterator it = m_changed_textures.begin();it!=m_changed_textures.end();++it) {
            (*it)->DiscardInternal();
        }
        m_changed_textures.clear();
    }
    
    void TexturePool::Clear() {
        m_changed_textures.clear();
        m_textures.clear();
    }
}
