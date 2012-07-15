/*
 *  sb_object.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 08.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */


#include "sb_scene_object.h"
#include "sb_assert.h"

SB_META_DECLARE_OBJECT(Sandbox::SceneObject, Sandbox::meta::object)

namespace Sandbox {


	SceneObject::SceneObject() : m_parent(0) ,m_visible(true){
	}
	
	SceneObject::~SceneObject() {
		sb_assert(m_parent==0);
	}
	
	void SceneObject::SetParent(Container* parent) {
		m_parent = parent;
	}
    
    bool SceneObject::HandleTouch( const TouchInfo& ) {
        return false;
    }
}
