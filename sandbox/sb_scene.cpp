/*
 *  sb_scene.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 08.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_scene.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::Scene, Sandbox::Container)

namespace Sandbox {

    Scene::Scene() : m_scale(1.0f) {
        
    }

	void Scene::Draw(Graphics& g) const {
        if (!GetVisible()) return;
        Transform2d tr = g.GetTransform();
        g.SetTransform(tr.scaled(m_scale,m_scale));
        Container::Draw(g);
        g.SetTransform(tr);
	}

    void Scene::Update( float dt ) {
        if (!GetVisible()) return;
        Container::Update(dt);
    }
    
    void Scene::SetScale( float scale ) {
        m_scale = scale;
    }
    
    float Scene::GetScale() const {
        return m_scale;
    }
}
