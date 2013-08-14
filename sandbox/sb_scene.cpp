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


	void Scene::Draw(Graphics& g) const {
        if (!GetVisible()) return;
        Container::Draw(g);
	}

    void Scene::Update( float dt ) {
        if (!GetVisible()) return;
        Container::Update(dt);
    }
}
