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

#include "meta/sb_meta.h"

namespace Sandbox {
	
	class Thread : public meta::object {
        SB_META_OBJECT
	public:
		virtual ~Thread();
		virtual bool Update(float dt) = 0;
    protected:
        Thread();
	};
	typedef sb::shared_ptr<Thread> ThreadPtr;
	
}

#endif /*SB_THREAD_H*/
