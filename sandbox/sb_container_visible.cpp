/*
 *  sb_container_visible.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 23.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_container_visible.h"
SB_META_DECLARE_OBJECT(Sandbox::ContainerVisible, Sandbox::Container)

namespace Sandbox {

	ContainerVisible::ContainerVisible(const sb::function<bool()>& func) : m_func(func),m_invert(false) {
	}
	
	void ContainerVisible::Draw(Graphics& g) const {
		if (m_func && (m_func()!=m_invert))
			DrawChilds(g);
	}

}
