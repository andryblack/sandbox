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
    
    class ControllerOffsets : public Controller {
    public:
        explicit ControllerOffsets(const ControllerPtr& child);
        ~ControllerOffsets();
        void Set(float k);
        void SetBegin(float phase) { m_begin = phase;}
        float GetBegin() const { return m_begin;}
        void SetEnd(float phase) { m_end = phase;}
        float GetEnd() const { return m_end;}
    private:
        ControllerPtr m_child;
        float m_begin;
        float m_end;
    };
}

#endif /*SB_CONTROLLER_PHASE_H*/
