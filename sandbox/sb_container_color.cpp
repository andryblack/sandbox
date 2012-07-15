/*
 *  sb_container_color.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 19.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_container_color.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::ContainerColor, Sandbox::Container)

namespace Sandbox {


	ContainerColor::ContainerColor() : m_color(1.0f,1.0f,1.0f,1.0f){
	}
	
	ContainerColor::~ContainerColor() {
	}
	
	void ContainerColor::Draw(Graphics& g) const {
		if (m_color.a>1.0f/256) {
			Color old = g.GetColor();
			g.SetColor(old*m_color);
			DrawChilds(g);
			g.SetColor(old);
		}
	}
}
