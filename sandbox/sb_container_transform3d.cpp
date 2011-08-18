/*
 *  sb_container_transform3d.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 11.05.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_container_transform3d.h"
#include "sb_graphics.h"

namespace Sandbox {

	ContainerTransform3d::ContainerTransform3d()  {
		m_projection = Matrix4f::identity();
		m_view = Matrix4f::identity();
	}
	
	ContainerTransform3d::~ContainerTransform3d() {
	}
	
	void ContainerTransform3d::Draw(Graphics& g) const {
		Matrix4f old_p = g.GetProjectionMatrix();
		Matrix4f old_v = g.GetViewMatrix();
		g.SetProjectionMatrix(m_projection);
		g.SetViewMatrix(m_view);
		DrawContent(g);
		g.SetProjectionMatrix(old_p);
		g.SetViewMatrix(old_v);
	}
}
