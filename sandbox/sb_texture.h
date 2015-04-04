/*
 *  sb_texture.h
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_TEXTURE_H
#define SB_TEXTURE_H

#include <sbstd/sb_intrusive_ptr.h>
#include <sbstd/sb_string.h>
#include <ghl_types.h>

namespace GHL {
    struct Texture;
}

namespace Sandbox {
    
	class Resources;
    class Bitmask;
    typedef sb::intrusive_ptr<Bitmask> BitmaskPtr;

	
    class Texture : public sb::ref_countered_base_not_copyable {
	private:
        GHL::Texture*   m_texture;
        sb::string              m_file;
        GHL::UInt32             m_original_w;
        GHL::UInt32             m_original_h;
        GHL::UInt32             m_width;
        GHL::UInt32             m_height;
        mutable size_t          m_live_ticks;
        bool    m_filtered;
        bool    m_tiled;
        bool    m_need_premultiply;
        BitmaskPtr              m_bitmask;
	public:
		explicit Texture(const sb::string& file, bool premul, GHL::UInt32 w, GHL::UInt32 h);
        explicit Texture(GHL::Texture* tes,GHL::UInt32 w=0, GHL::UInt32 h=0);
		~Texture();
		GHL::Texture* Present(Resources* resources);
        BitmaskPtr GetBitmask(Resources* resources);
		void SetFiltered(bool f);
        bool GetFiltered() const { return m_filtered; }
        void SetTiled(bool t) ;
        bool GetTiled() const { return m_tiled; }
        GHL::UInt32 GetWidth() const { return m_width; }
        GHL::UInt32 GetHeight() const { return m_height; }
	    GHL::UInt32 GetOriginalWidth() const { return m_original_w; }
        GHL::UInt32 GetOriginalHeight() const { return m_original_h; }
        size_t GetLiveTicks() const { return m_live_ticks; }
        size_t Release();
        size_t GetMemoryUsage() const;
        const sb::string& GetName() const { return m_file; }
        
        void SetTextureSize(GHL::UInt32 tw,GHL::UInt32 th);
	};
	typedef sb::intrusive_ptr<Texture> TexturePtr;
}

#endif /*SB_TEXTURE_H*/
