/*
 *  sb_container_viewport.h
 *  SR
 *
 *  Created by Андрей Куницын on 29.05.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_VIEWPORT_H
#define SB_CONTAINER_VIEWPORT_H

#include "sb_container.h"
#include "sb_rect.h"

namespace Sandbox {
	
	class ContainerViewport : public Container {
        SB_META_OBJECT
    public:
		ContainerViewport();
		~ContainerViewport();
		
		void Draw(Graphics& g) const SB_OVERRIDE;
		
		void Set(const Recti& r) { m_rect = r;}
		const Recti& Get() const { return m_rect;}
		
		void SetX(int x) { m_rect.x = x; }
		void SetY(int y) { m_rect.y = y; }
		void SetW(int w) { m_rect.w = w; }
		void SetH(int h) { m_rect.h = h; }
		
		void SetOrtho(bool o) { m_set_ortho = o;}
		bool GetOrtho() const { return m_set_ortho;}
	private:
		Recti m_rect;
		bool  m_set_ortho;
	};
	typedef sb::intrusive_ptr<ContainerViewport> ContainerViewportPtr;
	
}

#endif /*SB_CONTAINER_VIEWPORT_H*/
