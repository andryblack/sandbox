/*
 *  sb_thread.h
 *  SR
 *
 *  Created by Андрей Куницын on 15.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_THREAD_H
#define SB_THREAD_H

#include "sb_shared_ptr.h"

namespace Sandbox {
	
	class Thread {
	public:
		virtual ~Thread() {}
		virtual bool Update(float dt) = 0;
	};
	typedef shared_ptr<Thread> ThreadPtr;
	
}

#endif /*SB_THREAD_H*/