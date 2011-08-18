/*
 *  sb_threads_mgr.h
 *  SR
 *
 *  Created by Андрей Куницын on 15.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_THREADS_MGR_H
#define SB_THREADS_MGR_H

#include "sb_thread.h"
#include <vector>

namespace Sandbox {
	
	class ThreadsMgr : public Thread {
	public:
		ThreadsMgr();
		~ThreadsMgr();
		
		bool Update(float dt);
		
		void AddThread(const ThreadPtr& t);
		void RemoveThread(const ThreadPtr& t);
		void Clear();
	private:
		std::vector<ThreadPtr> m_threads;
		std::vector<ThreadPtr> m_added_threads;
		std::vector<ThreadPtr> m_removed_threads;
	};
}

#endif /*SB_THREADS_MGR_H*/
