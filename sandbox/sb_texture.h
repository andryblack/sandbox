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

#include "sb_shared_ptr.h"
#include <ghl_texture.h>

namespace Sandbox {
	
	class Texture {
	private:
		GHL::Texture* m_texture;
	public:
		explicit Texture(GHL::Texture* texture) : 
			m_texture(texture){}
		~Texture() { if (m_texture) m_texture->Release();}
		const GHL::Texture* Present() const { return m_texture;}
		void SetFiltered(bool f) { 
			m_texture->SetMinFilter(f?GHL::TEX_FILTER_LINEAR:GHL::TEX_FILTER_NEAR);
			m_texture->SetMagFilter(f?GHL::TEX_FILTER_LINEAR:GHL::TEX_FILTER_NEAR);
		}
		float GetWidth() const { return float(m_texture->GetWidth());}
		float GetHeight() const { return float(m_texture->GetHeight());}
		GHL::Texture* GetNative() const { return m_texture;}
	};
	typedef shared_ptr<Texture> TexturePtr;
}

#endif /*SB_TEXTURE_H*/
