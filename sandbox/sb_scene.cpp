/*
 *  sb_scene.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 08.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_scene.h"


namespace Sandbox {


	void Scene::Draw(Graphics& g) const {
		if (m_root) m_root->DoDraw(g);
	}

}
