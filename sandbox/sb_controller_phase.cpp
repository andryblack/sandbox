/*
 *  sb_controller_phase.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 21.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_controller_phase.h"


namespace Sandbox {

	ControllerPhase::ControllerPhase(const ControllerPtr& child) : m_child(child),m_phase(0.0f) {
	}
	
	ControllerPhase::~ControllerPhase() {
	}
	
	void ControllerPhase::Set(float k) {
		if (k<=(1.0f-m_phase)) {
			k = k+m_phase;
		} else {
			k = k - (1.0f-m_phase);
		}
		m_child->Set(k);
	}
    
    ControllerOffsets::ControllerOffsets(const ControllerPtr& child) : m_child(child),m_begin(0.0f),m_end(0.0f) {
    }
    
    ControllerOffsets::~ControllerOffsets() {
    }

    void ControllerOffsets::Set(float k) {
        if (k<=m_begin) {
            k = 0.0f;
        } else if (k>=(1.0f-m_end)) {
            k = 1.0f;
        } else {
            k = (k-m_begin) / (1.0f - m_begin - m_end);
        }
        m_child->Set(k);
    }
}
