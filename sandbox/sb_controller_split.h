/*
 *  sb_controller_split.h
 *  SR
 *
 *  Created by Андрей Куницын on 21.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTROLLER_SPLIT_H
#define SB_CONTROLLER_SPLIT_H

#include "sb_controller.h"
#include <vector>

namespace Sandbox {
	
	class ControllerSplit : public Controller {
	public:
		ControllerSplit();
		~ControllerSplit();
		
		void Set(float k);
		
		void Reserve(size_t size);
		void AddController(const ControllerPtr& controller);
		void RemoveController(const ControllerPtr& controller);
		void Clear();
	private:
		std::vector<ControllerPtr> m_controllers;
	};
}
#endif /*SB_CONTROLLER_SPLIT_H*/
