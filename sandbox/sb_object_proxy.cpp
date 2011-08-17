/*
 *  sb_object_proxy.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 23.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_object_proxy.h"

namespace Sandbox {

	ObjectProxy::ObjectProxy(const function<void(Graphics&)>& func) : Object(), m_func(func) {
	}
	
	void ObjectProxy::Draw(Graphics& g) const {
		if (m_func) m_func(g);
	}
	
}
