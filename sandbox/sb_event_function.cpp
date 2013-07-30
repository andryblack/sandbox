/*
 *  sb_event_function.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 10.04.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */


#include "sb_event_function.h"

namespace Sandbox {
	
	
	EventFunction::EventFunction( const sb::function<void()>& f) : m_func(f) {
	}

	void EventFunction::Emmit() {
		if (m_func) m_func();
	}
	
	EventPtr EventFunction::Create(const sb::function<void()>& f) {
        return sb::make_shared<EventFunction>(f);
	}

}
