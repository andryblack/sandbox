/*
 *  sb_container_viewport.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 29.05.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_container_viewport.h"
#include "sb_graphics.h"
#include "sb_matrix4.h"

SB_META_DECLARE_OBJECT(Sandbox::ContainerViewport, Sandbox::Container)

namespace Sandbox {

	ContainerViewport::ContainerViewport() {
		m_rect.x = m_rect.y = m_rect.w = m_rect.h = 0;
		m_set_ortho = true;
	}
	
	ContainerViewport::~ContainerViewport() {
	}
	
	void ContainerViewport::Draw(Graphics& g) const {
		if (m_rect.w!=0 && m_rect.h!=0) {
			Matrix4f oldM;
			if (m_set_ortho) {
				oldM = g.GetProjectionMatrix();
				g.SetProjectionMatrix(Matrix4f::ortho(0,float(m_rect.w),
													  float(m_rect.h),0,-10,10));
			}
			Recti old = g.GetViewport();
			g.SetViewport(m_rect);
			DrawChilds(g);
			g.SetViewport(old);
			if (m_set_ortho)
				g.SetProjectionMatrix(oldM);
		} else {
			DrawChilds(g);
		}
	}
	
}
