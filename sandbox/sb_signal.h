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

#include "sb_event.h"

#include <sbstd/sb_intrusive_ptr.h>
#include "luabind/sb_luabind_stack.h"

namespace Sandbox {

	class Signal : public sb::ref_countered_base {
	public:
		virtual ~Signal() {}
        void Emmit() { Emmit(EventPtr()); }
		virtual void Emmit(const EventPtr& event) = 0;
        virtual void Clear() {}
	protected:
	};
	typedef sb::intrusive_ptr<Signal> SignalPtr;
	
    namespace luabind {
        
        template <>
        struct stack<SignalPtr> {
            static SignalPtr get( lua_State* L, int idx);
            static void push( lua_State* L, const SignalPtr& val ) {
                stack_push_impl( L,val );
            }
        };

    }
}

#endif /*SB_EVENT_H*/
