/*
 *  sb_controller_elastic.h
 *  SR
 *
 *  Created by Андрей Куницын on 28.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTROLLER_ELASTIC_H
#define SB_CONTROLLER_ELASTIC_H

#include "sb_controller.h"

namespace Sandbox {
	
	class ControllerElastic : public Controller {
	public:
		explicit ControllerElastic(const ControllerPtr& child);
		~ControllerElastic();
		void Set(float k);
		float GetHard() const { return m_hard;}
		void SetHard(float a) { m_hard = a;}
		float GetEnd() const { return m_end;}
		void SetEnd(float e) { m_end = e;}
		void SetPhases(size_t p) { m_phases = p;}
		size_t GetPhases() const { return m_phases;}
	private:
		ControllerPtr m_child;
		float m_hard;
		float m_end;
		float m_amplitude;
		size_t	m_phases;
	};
    
    class ControllerSinus : public Controller {
    public:
        explicit ControllerSinus(const ControllerPtr& child);
        void Set(float k);
    private:
        ControllerPtr m_child;
    };
    	
}
#endif /*SB_CONTROLLER_ELASTIC_H*/
