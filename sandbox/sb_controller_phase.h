/*
 *  sb_controller_phase.h
 *  SR
 *
 *  Created by Андрей Куницын on 21.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTROLLER_PHASE_H
#define SB_CONTROLLER_PHASE_H

#include "sb_controller.h"

namespace Sandbox {
	
	class ControllerPhase : public Controller {
	public:
		explicit ControllerPhase(const ControllerPtr& child);
		~ControllerPhase();
		void Set(float k);
		void SetPhase(float phase) { m_phase = phase;}
		float GetPhase() const { return m_phase;}
	private:
		ControllerPtr m_child;
		float m_phase;
	};
}

#endif /*SB_CONTROLLER_PHASE_H*/
