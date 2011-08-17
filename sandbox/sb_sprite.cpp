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


namespace Sandbox {
	
	Sprite::Sprite() {
	}
	
	Sprite::~Sprite() {
	}
	
	void Sprite::Draw(Graphics& g) const {
		g.DrawImage(m_image,m_pos);
	}

}

