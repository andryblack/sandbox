/*
 *  sb_processor.h
 *  SR
 *
 *  Created by Андрей Куницын on 18.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_PROCESSOR_H
#define SB_PROCESSOR_H

#include "sb_thread.h"
#include "sb_controller.h"
#include "sb_signal.h"

namespace Sandbox {
	
	class Processor : public Thread
	{
	public:
		Processor();
		virtual ~Processor();
		
		bool Update(float d);
		
		const ControllerPtr& GetController() const { return m_controller;}
		void SetController(const ControllerPtr& controller) { m_controller = controller;}
		
		bool GetLoop() const { return m_loop;}
		void SetLoop(bool l) { m_loop = l;}
		
		void SetOnEnd(const SignalPtr& e) { m_end_signal = e;}
        const SignalPtr& GetOnEnd() const { return m_end_signal; }
		
		float GetTime() const { return m_all_time;}
		void SetTime(float t) { m_all_time = t;}
        
        void Start();
		void Stop();
        void Restart();
        
		bool GetInverted() const { return m_inverted;}
		void SetInverted(bool i) { m_inverted = i;}
        
        virtual void Clear();
	private:
		ControllerPtr	m_controller;
		bool			m_loop;
		SignalPtr		m_end_signal;
		float			m_all_time;
		bool			m_started;
		float			m_time;
		bool		m_inverted;
	};
	
}

#endif /*SB_PROCESSOR_H*/
