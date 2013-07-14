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

    bool Scene::HandleTouch( const TouchInfo& touch ) {
        if (!GetVisible()) return false;
        if (WidgetPtr w = m_focus_widget.lock()) {
            if (w->HandleTouch(touch))
                return true;
        }
        return Container::HandleTouch(touch);
    }
    
    void Scene::Update( float dt ) {
        if (!GetVisible()) return;
        Container::Update(dt);
        if (WidgetPtr w = m_focus_widget.lock()) {
            if (!w->GetVisible()) {
                m_focus_widget = WidgetWeakPtr();
            }
        }
    }
}
