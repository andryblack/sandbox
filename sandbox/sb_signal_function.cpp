/*
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */


#include "sb_signal_function.h"

namespace Sandbox {
	
	
	SignalFunction::SignalFunction( const sb::function<void(const EventPtr& e)>& f) : m_func(f) {
	}

	void SignalFunction::Emmit(const EventPtr& e) {
		if (m_func) m_func(e);
	}
	
	SignalPtr SignalFunction::Create(const sb::function<void(const EventPtr& e)>& f) {
        return SignalPtr( new SignalFunction(f) );
	}

}
