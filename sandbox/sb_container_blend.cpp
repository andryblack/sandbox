/*
 *  sb_container_blend.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 11.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_container_blend.h"

SB_META_DECLARE_OBJECT(Sandbox::ContainerBlend, Sandbox::Container)
SB_META_DECLARE_OBJECT(Sandbox::ContainerMask, Sandbox::Container)

namespace Sandbox {

	ContainerBlend::ContainerBlend() : m_mode( BLEND_MODE_ALPHABLEND ) {
	}
	
	void ContainerBlend::Draw(Graphics& g) const {
		BlendMode m = g.GetBlendMode();
		g.SetBlendMode(m_mode);
        Container::Draw(g);
		g.SetBlendMode(m);
	}
    
    ContainerMask::ContainerMask() : m_mode( MASK_MODE_NONE ) {
	}
	
	void ContainerMask::Draw(Graphics& g) const {
		MaskMode m = g.GetMaskMode();
        TexturePtr t = g.GetMaskTexture();
        Transform2d tr = g.GetMaskTransform();
		g.SetMask(m_mode, m_texture, m_transform);
        Container::Draw(g);
		g.SetMask(m, t, tr);
	}
}
