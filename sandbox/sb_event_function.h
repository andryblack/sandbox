/*
 *  sb_event_function.h
 *  SR
 *
 *  Created by Андрей Куницын on 10.04.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_EVENT_FUNCTION
#define SB_EVENT_FUNCTION

#include "sb_event.h"
#include <sbstd/sb_function.h>

namespace Sandbox {
	
	class EventFunction : public Event {
	public:
		explicit EventFunction( const sb::function<void()>& f);
		virtual void Emmit();
		static EventPtr Create( const sb::function<void()>& f);
	private:
		sb::function<void()>	m_func;
	};
}

#endif /*SB_EVENT_FUNCTION*/
