/*
 *  sb_object.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 08.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */


#include "sb_object.h"
#include "sb_assert.h"

namespace Sandbox {


	Object::Object() : m_parent(0) ,m_visible(true){
	}
	
	Object::~Object() {
		sb_assert(m_parent==0);
	}
	
	void Object::SetParent(Container* parent) {
		m_parent = parent;
	}
}