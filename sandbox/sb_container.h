/*
 *  sb_container.h
 *  SR
 *
 *  Created by Андрей Куницын on 11.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_H
#define SB_CONTAINER_H

#include "sb_object.h"
#include <vector>
namespace Sandbox {

	class Container : public Object {
	public:
		Container();
		~Container();
		
		void Reserve(size_t size);
		void Draw(Graphics& g) const;
		void AddObject(const ObjectPtr& o);
		void RemoveObject(const ObjectPtr& obj);
		void Clear();
	protected:
		void DrawContent(Graphics& g) const;
		std::vector<ObjectPtr> m_objects;
	};
	typedef shared_ptr<Container> ContainerPtr;
}

#endif /*SB_CONTAINER_H*/