/*
 *  sb_controller_map.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 21.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_controller_map.h"

namespace Sandbox {

	ControllerMap::ControllerMap(const ControllerPtr& child) : m_child(child) {
		SetPoint(0.0f,0.0f);
		SetPoint(1.0f,1.0f);
	}
	ControllerMap::~ControllerMap() {
	}
	
	void ControllerMap::Set(float v) {
		float p_low = m_points[0].pos;
		float v_low = m_points[0].val;
		for (size_t i=1;i<m_points.size();i++) {
			float p_hi = m_points[i].pos;
			float v_hi = m_points[i].val;
			if (v>=p_low && v<=p_hi) {
				float p_len = p_hi-p_low;
				float k = (v-p_low)/(p_len>0.00001f ? p_len : 1.0f);
				v = v_hi * k + v_low*(1.0f-k);
				break;
			}
			p_low = p_hi;
			v_low = v_hi;
		}
		m_child->Set(v);
	}

	void ControllerMap::SetPoint(float pos,float val) {
		std::vector<Point>::iterator it = m_points.begin();
		while (it!=m_points.end()) {
			if (it->pos==pos) {
				it->val = val;
				return;
			} 
			if (it->pos>pos)
				break;
			it++;
		}
		Point p = {pos,val};
		m_points.insert(it,p);
	}
}
