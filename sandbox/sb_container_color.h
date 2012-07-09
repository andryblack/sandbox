/*
 *  sb_container_color.h
 *  SR
 *
 *  Created by Андрей Куницын on 19.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_COLOR_H
#define SB_CONTAINER_COLOR_H

#include "sb_container.h"
#include "sb_color.h"

namespace Sandbox {
	
	class Graphics;
	
	class ContainerColor : public Container {
	public:
		ContainerColor();
		~ContainerColor();
		void Draw(Graphics& g) const;
		
		void SetColor(const Color& c) { m_color = c;}
		const Color& GetColor() const { return m_color; }
		void SetAlpha(float a) { m_color.a = a;}
		float GetAlpha() const { return m_color.a;}
	private:
		Color	m_color;
	};
	typedef sb::shared_ptr<ContainerColor> ContainerColorPtr;
}

#endif /*SB_CONTAINER_COLOR_H*/
