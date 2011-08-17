/*
 *  sb_controller_bidirect.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 21.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_controller_bidirect.h"

namespace Sandbox {

	ControllerBidirect::ControllerBidirect(const ControllerPtr& child) : m_child(child) {
	}
	
	ControllerBidirect::~ControllerBidirect() {
	}
	
	void ControllerBidirect::Set(float k) {
		if (k<=0.5f) m_child->Set(k*2);
		else {
			m_child->Set(2.0f-k*2);
		}

	}
}