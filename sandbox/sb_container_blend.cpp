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

namespace Sandbox {

	ContainerBlend::ContainerBlend() : m_mode( BLEND_MODE_ALPHABLEND ) {
	}
	
	void ContainerBlend::Draw(Graphics& g) const {
		BlendMode m = g.GetBlendMode();
		g.SetBlendMode(m_mode);
        Container::Draw(g);
		g.SetBlendMode(m);
	}
    
    
}
