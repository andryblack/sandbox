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

#include <sbstd/sb_shared_ptr.h>
#include "sb_notcopyable.h"
#include <sbstd/sb_string.h>
#include <ghl_types.h>

namespace GHL {
    struct Texture;
}

namespace Sandbox {
    
	class Resources;
	
    class Texture : public NotCopyable {
	private:
	    mutable GHL::Texture*   m_texture;
        sb::string              m_file;
        GHL::UInt32             m_original_w;
        GHL::UInt32             m_original_h;
        mutable size_t          m_live_ticks;
        bool    m_filtered;
        bool    m_tiled;
        bool    m_need_premultiply;
	public:
		explicit Texture(const sb::string& file, bool premul, GHL::UInt32 w, GHL::UInt32 h);
        explicit Texture(GHL::Texture* tes,GHL::UInt32 w=0, GHL::UInt32 h=0);
		~Texture();
		const GHL::Texture* Present(Resources* resources) const;
		void SetFiltered(bool f);
        bool GetFiltered() const { return m_filtered; }
        void SetTiled(bool t) ;
        bool GetTiled() const { return m_tiled; }
	    GHL::UInt32 GetOriginalWidth() const { return m_original_w; }
        GHL::UInt32 GetOriginalHeight() const { return m_original_h; }
        size_t GetLiveTicks() const { return m_live_ticks; }
        size_t Release();
        size_t GetMemoryUsage() const;
        const sb::string& GetName() const { return m_file; }
	};
	typedef sb::shared_ptr<Texture> TexturePtr;
}

#endif /*SB_TEXTURE_H*/
