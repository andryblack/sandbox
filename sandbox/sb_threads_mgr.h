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
        SB_META_OBJECT
	public:
		ThreadsMgr();
		~ThreadsMgr();
		
		bool Update(float dt);
		
		void AddThread(const ThreadPtr& t);
		void RemoveThread(const ThreadPtr& t);
		void Clear();
        
        bool GetDropEmpty() const { return m_drop_empty; }
        void SetDropEmpty(bool d) { m_drop_empty = d; }
        
        float GetSpeed() const { return m_speed; }
        void SetSpeed(float s) { m_speed = s; }
        
   private:
		std::vector<ThreadPtr> m_threads;
		std::vector<ThreadPtr> m_added_threads;
		std::vector<ThreadPtr> m_removed_threads;
        bool    m_drop_empty;
        float   m_speed;
	};
    
    typedef sb::intrusive_ptr<ThreadsMgr> ThreadsMgrPtr;
    
    class PermanentThreadsMgr : public ThreadsMgr {
        SB_META_OBJECT
    public:
        bool Update(float dt);
    };
}

#endif /*SB_THREADS_MGR_H*/
