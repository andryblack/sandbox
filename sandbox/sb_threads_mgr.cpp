/*
 *  sb_threads_mgr.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 15.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_threads_mgr.h"
#include <algorithm>

SB_META_DECLARE_OBJECT(Sandbox::ThreadsMgr, Sandbox::Thread)
SB_META_DECLARE_OBJECT(Sandbox::PermanentThreadsMgr, Sandbox::ThreadsMgr)

namespace Sandbox {

    ThreadsMgr::ThreadsMgr() : m_drop_empty(true) {
	}
	
	ThreadsMgr::~ThreadsMgr() {
	}
	
	bool ThreadsMgr::Update(float dt) {
		for (size_t i=0;i<m_removed_threads.size();i++) {
			std::vector<ThreadPtr>::iterator it = std::find(m_threads.begin(),m_threads.end(),m_removed_threads[i]);
			if (it!=m_threads.end())
				m_threads.erase(it);
		}
		m_removed_threads.clear();
		for (size_t i=0;i<m_added_threads.size();i++) {
			std::vector<ThreadPtr>::iterator it = std::find(m_threads.begin(),m_threads.end(),m_added_threads[i]);
			if (it==m_threads.end())
				m_threads.push_back(m_added_threads[i]);
		}
		m_added_threads.clear();
		std::vector<ThreadPtr>::iterator i = m_threads.begin();
		while (i!=m_threads.end()) {
			if ((*i)->Update(dt))
				i = m_threads.erase(i);
			else
				i++;
		}
		return m_threads.empty() && m_removed_threads.empty() && m_added_threads.empty() && m_drop_empty;
	}
	
	void ThreadsMgr::AddThread(const ThreadPtr& t) {
		sb_assert( t && "null thread" );
		//std::vector<ThreadPtr>::iterator it = std::find(m_threads.begin(),m_threads.end(),t);
		std::vector<ThreadPtr>::iterator it = std::find(m_added_threads.begin(),m_added_threads.end(),t);
		if (it==m_added_threads.end()) {
			it = std::find(m_removed_threads.begin(),m_removed_threads.end(),t);
			if (it!=m_removed_threads.end())
				m_removed_threads.erase(it);
			m_added_threads.push_back(t);
		}
	}
	void ThreadsMgr::RemoveThread(const ThreadPtr& t) {
		sb_assert( t && "null thread" );
		std::vector<ThreadPtr>::iterator it = std::find(m_added_threads.begin(),m_added_threads.end(),t);
		if (it!=m_added_threads.end())
			m_added_threads.erase(it);
		it = std::find(m_removed_threads.begin(),m_removed_threads.end(),t);
		if (it==m_removed_threads.end())
			m_removed_threads.push_back(t);
		
	}
	void ThreadsMgr::Clear() {
		m_added_threads.clear();
		for (size_t i=0;i<m_threads.size();i++) {
			std::vector<ThreadPtr>::iterator it = std::find(m_removed_threads.begin(),m_removed_threads.end(),m_threads[i]);
			if (it==m_removed_threads.end())
				m_removed_threads.push_back(m_threads[i]);
		}
	}

    bool PermanentThreadsMgr::Update(float dt) {
        ThreadsMgr::Update(dt);
        return false;
    }
}
