/*
 *  sb_processor.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 18.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_processor.h"

namespace Sandbox {

	
	Processor::Processor() : m_loop(false),m_all_time(1.0f),m_started(false),m_time(0.0f),m_inverted(false){
	}
	
	Processor::~Processor() {
	}
	
	bool Processor::Update(float dt) {
		if (m_started) {
			m_time+=dt;
			if (m_time>=m_all_time) {
				if (m_loop) {
					m_time -= m_all_time;
				} else {
					m_time = m_all_time;
					m_started = false;
					if (m_end_event) m_end_event->Emmit();
				}
			}
			float k = m_time/m_all_time;
			if (m_controller) m_controller->Set(m_inverted ? 1.0f - k : k);
		}
		return !m_started;
	}
	
	void Processor::Start() {
		if (!m_started) {
			m_time = 0.0f;
			m_started = true;
			if (m_controller) m_controller->Set(m_inverted ? 1.0f : 0.0f);
		}
	}
	
	void Processor::Stop() {
		m_started = false;
	}

}
