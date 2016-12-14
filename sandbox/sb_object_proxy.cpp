/*
 *  sb_object_proxy.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 23.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_object_proxy.h"

SB_META_DECLARE_OBJECT(Sandbox::ObjectDrawProxy, Sandbox::SceneObject)
SB_META_DECLARE_OBJECT(Sandbox::SceneObjectProxy, Sandbox::SceneObject)

namespace Sandbox {

	ObjectDrawProxy::ObjectDrawProxy(const sb::function<void(Graphics&)>& func) : SceneObject(), m_func(func) {
	}
	
	void ObjectDrawProxy::Draw(Graphics& g) const {
		if (m_func) m_func(g);
	}
	
    
    SceneObjectProxy::SceneObjectProxy(const SceneObjectPtr& ref) : m_ref(ref) {
        
    }
    
    void SceneObjectProxy::Draw(Graphics &g) const {
        if (m_ref) {
            m_ref->Draw(g);
        }
    }
}
