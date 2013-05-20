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

#include <sbtl/sb_shared_ptr.h>
#include "sb_notcopyable.h"
#include <ghl_texture.h>
#include <sbtl/sb_string.h>

namespace Sandbox {
	
	class Texture : public NotCopyable {
	private:
		GHL::Texture* m_texture;
        GHL::UInt32 m_original_w;
        GHL::UInt32 m_original_h;
	public:
		explicit Texture(GHL::Texture* texture, GHL::UInt32 w=0, GHL::UInt32 h=0) : 
			m_texture(texture),m_original_w(w),m_original_h(h){
            if (m_texture) {
                if (m_original_w==0) m_original_w = m_texture->GetWidth();
                if (m_original_h==0) m_original_h = m_texture->GetHeight();
            }
        }
		~Texture() { if (m_texture) m_texture->Release();}
		const GHL::Texture* Present() const { return m_texture;}
		void SetFiltered(bool f) { 
			m_texture->SetMinFilter(f?GHL::TEX_FILTER_LINEAR:GHL::TEX_FILTER_NEAR);
			m_texture->SetMagFilter(f?GHL::TEX_FILTER_LINEAR:GHL::TEX_FILTER_NEAR);
		}
        bool GetFiltered() const {
            return m_texture->GetMagFilter() != GHL::TEX_FILTER_NEAR;
        }
        void SetTiled(bool t) {
			m_texture->SetWrapModeU(t?GHL::TEX_WRAP_REPEAT:GHL::TEX_WRAP_CLAMP);
			m_texture->SetWrapModeV(t?GHL::TEX_WRAP_REPEAT:GHL::TEX_WRAP_CLAMP);
        }
        bool GetTiled() const {
            return m_texture->GetWrapModeU() != GHL::TEX_WRAP_CLAMP;
        }
		GHL::UInt32 GetWidth() const { return m_texture->GetWidth();}
		GHL::UInt32 GetHeight() const { return m_texture->GetHeight();}
        GHL::UInt32 GetOriginalWidth() const { return m_original_w; }
        GHL::UInt32 GetOriginalHeight() const { return m_original_h; }
		GHL::Texture* GetNative() const { return m_texture;}
	};
	typedef sb::shared_ptr<Texture> TexturePtr;
}

#endif /*SB_TEXTURE_H*/
