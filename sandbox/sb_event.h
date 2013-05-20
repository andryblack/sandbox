/*
 *  sb_event.h
 *  SR
 *
 *  Created by Андрей Куницын on 14.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_EVENT_H
#define SB_EVENT_H

#include <sbtl/sb_shared_ptr.h>

namespace Sandbox {

	class Event {
	public:
		virtual ~Event() {}
		virtual void Emmit() = 0;
	protected:
	};
	typedef sb::shared_ptr<Event> EventPtr;
	
}

#endif /*SB_EVENT_H*/
