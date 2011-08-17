/*
 *  sb_container_visible.h
 *  SR
 *
 *  Created by Андрей Куницын on 23.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_VISIBLE_H
#define SB_CONTAINER_VISIBLE_H

#include "sb_container.h"
#include "sb_function.h"

namespace Sandbox {
	
	class ContainerVisible : public Container {
	public:
		explicit ContainerVisible(const function<bool()>& func);
		void Draw(Graphics& g) const;
		bool GetInvert() const { return m_invert;}
		void SetInvert(bool i) { m_invert = i;}
	private:
		function<bool()>	m_func;
		bool	m_invert;
	};
	typedef shared_ptr<ContainerVisible> ContainerVisiblePtr;
}

#endif /*SB_CONTAINER_VISIBLE_H*/