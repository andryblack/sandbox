/*
 *  sb_font.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 12.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_font.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::Font,Sandbox::meta::object)

namespace Sandbox {
	
	
		
	Font::Font() {
        m_size = 8.0f;
        m_height = 16.0f;
        m_baseline = 0.0f;
        m_xheight = m_height;
	}
	
	Font::~Font() {
	
	}
	
		
}
