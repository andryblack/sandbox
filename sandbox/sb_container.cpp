/*
 *  sb_container.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 11.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_container.h"
#include <algorithm>

namespace Sandbox {

	Container::Container() {
		
	}
	
	Container::~Container() {
		for (std::vector<ObjectPtr>::iterator i = m_objects.begin();i!=m_objects.end();i++) {
			(*i)->SetParent(0);
		}
	}
	void Container::Reserve(size_t size) {
		m_objects.reserve(size);
	}
	void Container::AddObject(const ObjectPtr& o) {
		sb_assert(o && "null object");
		if (Container* c=o->GetParent()) {
			c->RemoveObject(o);
		}
		o->SetParent(this);
		m_objects.push_back(o);
	}
	void Container::RemoveObject(const ObjectPtr& obj) {
		sb_assert( obj && "null object");
		std::vector<ObjectPtr>::iterator i = std::find(m_objects.begin(),m_objects.end(),obj);
		if (i!=m_objects.end()) {
			(*i)->SetParent(0);
			m_objects.erase(i);
		}
	}
	void Container::Clear() {
		for (size_t i=0;i<m_objects.size();i++) {
			m_objects[i]->SetParent(0);
		}
		m_objects.clear();
	}
	
	void Container::Draw(Graphics& g) const {
		DrawContent(g);
	}
	void Container::DrawContent(Graphics& g) const {
		for (std::vector<ObjectPtr>::const_iterator i = m_objects.begin();i!=m_objects.end();i++) {
			(*i)->DoDraw(g);
		}
	}
}