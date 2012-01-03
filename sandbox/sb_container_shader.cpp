/*
 *  sb_container_shader.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 13.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_container_shader.h"
#include "sb_graphics.h"

namespace Sandbox {

	ContainerShader::ContainerShader() : m_enabled(true){
	}
	
	ContainerShader::~ContainerShader() {
	}
	
	void ContainerShader::Draw(Graphics& g) const {
		if (m_enabled) {
			ShaderPtr ss = g.GetShader();
			g.SetShader(m_shader);
			DrawChilds(g);
			g.SetShader(ss);
		} else {
			DrawChilds(g);
		}
	}
	
	
}
