/*
 *  sb_object.h
 *  SR
 *
 *  Created by Андрей Куницын on 08.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_OBJECT_H
#define SB_OBJECT_H

#include "sb_shared_ptr.h"

namespace Sandbox {
	
	class Graphics;
	class Container;

	class Object {
	public:
		Object();
		virtual ~Object();
		virtual void Draw(Graphics& g) const = 0;
		void SetVisible(bool v) { m_visible = v;}
		bool GetVisible() const { return m_visible;}
		void DoDraw(Graphics& g) const {
			if (m_visible) Draw(g);
		}
	protected:
		friend class Container;
		Container* GetParent() const { return m_parent;}
	private:
		void SetParent(Container* parent);
		Container* m_parent;
		bool	m_visible;
	};
	typedef shared_ptr<Object> ObjectPtr;
}

#endif /*SB_OBJECT_H*/
