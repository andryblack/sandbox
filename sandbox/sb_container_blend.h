/*
 *  sb_container_blend.h
 *  SR
 *
 *  Created by Андрей Куницын on 11.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_BLEND_H
#define SB_CONTAINER_BLEND_H

#include "sb_container.h"
#include "sb_graphics.h"

namespace Sandbox {
	
	class ContainerBlend : public Container {
	public:
		ContainerBlend();
		void Draw(Graphics& g) const;
		void SetMode(BlendMode mode) { m_mode = mode;}
		BlendMode GetMode() const { return m_mode;}
	private:
		BlendMode	m_mode;
	};
}
#endif /*SB_CONTAINER_BLEND_H*/