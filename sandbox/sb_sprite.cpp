/*
 *  sb_sprite.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 08.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_sprite.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::Sprite, Sandbox::SceneObject)
SB_META_DECLARE_OBJECT(Sandbox::ColorizedSprite, Sandbox::Sprite)

namespace Sandbox {
	
	Sprite::Sprite() {
	}
	
	Sprite::~Sprite() {
	}
	
	void Sprite::Draw(Graphics& g) const {
        if (m_image)
            g.DrawImage(*m_image,m_pos);
	}

    void ColorizedSprite::Draw(Graphics& g) const {
        if (m_image)
            g.DrawImage(*m_image,m_pos,m_color);
    }
    
}

