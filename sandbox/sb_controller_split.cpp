/*
 *  sb_controller_split.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 21.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_controller_split.h"

namespace Sandbox {

	ControllerSplit::ControllerSplit(){
	}
	
	ControllerSplit::~ControllerSplit() {
	}
	
	void ControllerSplit::Set(float k) {
		for (size_t i=0;i<m_controllers.size();i++) {
			m_controllers[i]->Set(k);
		}
	}
	
	void ControllerSplit::Reserve(size_t size) {
		m_controllers.reserve(size);
	}
	void ControllerSplit::AddController(const ControllerPtr& controller) {
		m_controllers.push_back(controller);
	}
	
	void ControllerSplit::RemoveController(const ControllerPtr& controller) {
		for (std::vector<ControllerPtr>::iterator it = m_controllers.begin();it!=m_controllers.end();) {
			if ((*it)==controller)
				it = m_controllers.erase(it);
			else {
				it++;
			}

		}
	}
	void ControllerSplit::Clear() {
		m_controllers.clear();
	}

}