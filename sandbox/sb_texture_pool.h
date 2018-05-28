#ifndef SB_TEXTURE_POOL_H
#define SB_TEXTURE_POOL_H

#include <sbstd/sb_vector.h>
#include <sbstd/sb_intrusive_ptr.h>
#include "sb_texture.h"
#include <ghl_texture.h>

namespace Sandbox {

    class Resources;
    
    class TexturePool : public sb::ref_countered_base_not_copyable {
        GHL::UInt32         m_initial_texture_width;
        GHL::UInt32         m_initial_texture_height;
        Resources*  m_resources;
        sb::vector<GHL::Texture*> m_changed_textures;
        struct TextureData {
            GHL::TextureFormat fmt;
            TexturePtr  tex;
            int pos_x;
            int pos_y;
            int max_h;
        };
        typedef sb::vector<TextureData> TextureDataVector;
        TextureDataVector m_textures;
    public:
        TexturePool( Resources* resources  ) ;
        
        TexturePtr Alloc(int w, int h, int& x, int& y,GHL::Texture* &ntex,
                         GHL::TextureFormat fmt = GHL::TEXTURE_FORMAT_ALPHA);
        void Commit();
        void Clear();
    };
    typedef sb::intrusive_ptr<TexturePool> TexturePoolPtr;
}
#endif /*SB_TEXTURE_POOL_H*/
