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

#include <sbstd/sb_shared_ptr.h>
#include <luabind/sb_luabind_stack.h>

namespace Sandbox {

	class Event {
	public:
		virtual ~Event() {}
		virtual void Emmit() = 0;
	protected:
	};
	typedef sb::shared_ptr<Event> EventPtr;
	
    namespace luabind {
        
        template <>
        struct stack<EventPtr> {
            static EventPtr get( lua_State* L, int idx);
            static void push( lua_State* L, const EventPtr& val ) {
                stack_push_impl( L,val );
            }
        };

    }
}

#endif /*SB_EVENT_H*/
