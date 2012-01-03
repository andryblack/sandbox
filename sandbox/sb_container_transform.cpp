/*
 *  sb_container_transform.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 12.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_container_transform.h"
#include "sb_transform2d.h"
#include "sb_graphics.h"

namespace Sandbox {
	ContainerTransform::ContainerTransform() : m_scale_x(1.0f),m_scale_y(1.0f),m_angle(0.0f) {
	}
	
	ContainerTransform::~ContainerTransform() {
	}
	
	void ContainerTransform::Draw(Graphics& g) const {
		Transform2d old = g.GetTransform();
		Transform2d tr = old;
		tr.translate(m_translate).rotate(m_angle).scale(m_scale_x,m_scale_y);
		g.SetTransform(tr);
		DrawChilds(g);
		g.SetTransform(old);
	}
}
