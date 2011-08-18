/*
 *  sb_controller_map.h
 *  SR
 *
 *  Created by Андрей Куницын on 21.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTROLLER_MAP_H
#define SB_CONTROLLER_MAP_H

#include "sb_controller.h"
#include <vector>
namespace Sandbox {
	
	class ControllerMap : public Controller {
	public:
		explicit ControllerMap(const ControllerPtr& child);
		~ControllerMap();
		void Set(float v);
		void SetPoint(float pos,float val);
	private:
		ControllerPtr m_child;
		struct Point {
			float pos;
			float val;
		};
		std::vector<Point> m_points;
	};
}

#endif /*SB_CONTROLLER_MAP_H*/
