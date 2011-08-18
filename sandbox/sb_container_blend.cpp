/*
 *  sb_container_blend.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 11.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_container_blend.h"

namespace Sandbox {

	ContainerBlend::ContainerBlend() : m_mode( BLEND_MODE_ALPHABLEND ) {
	}
	
	void ContainerBlend::Draw(Graphics& g) const {
		BlendMode m = g.GetBlendMode();
		bool unset_shader = m_mode == BLEND_MODE_SCREEN;
		ShaderPtr sh;
		if (unset_shader) {
			sh = g.GetShader();
			g.SetShader(ShaderPtr());
		}
		g.SetBlendMode(m_mode);
		DrawContent(g);
		g.SetBlendMode(m);
		if (unset_shader)
			g.SetShader(sh);
	}
}
