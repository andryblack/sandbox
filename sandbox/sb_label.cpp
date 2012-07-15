/*
 *  sb_label.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 12.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_label.h"

SB_META_DECLARE_OBJECT(Sandbox::Label, Sandbox::SceneObject)

namespace Sandbox {

	Label::Label() {
	}
	Label::~Label() {
	}

	void Label::Draw(Graphics& g) const {
		if (m_font && !m_text.empty()) {
			m_font->Draw(g,m_pos,m_text.c_str(),m_align);
		}
	}
}
