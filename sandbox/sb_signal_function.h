/*
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_SIGNAL_FUNCTION_INCLUDED
#define SB_SIGNAL_FUNCTION_INCLUDED

#include "sb_signal.h"
#include <sbstd/sb_function.h>

namespace Sandbox {
	
	class SignalFunction : public Signal {
	public:
		explicit SignalFunction( const sb::function<void(const EventPtr& e)>& f);
		virtual void Emmit(const EventPtr& e);
		static SignalPtr Create( const sb::function<void(const EventPtr& e)>& f);
	private:
		sb::function<void(const EventPtr& e)>	m_func;
	};
}

#endif /*SB_SIGNAL_FUNCTION_INCLUDED*/
