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

    ThreadsMgr::ThreadsMgr() : m_drop_empty(true),m_speed(1.0f),m_lock_update(false) {
	}
	
	ThreadsMgr::~ThreadsMgr() {
	}
	
	bool ThreadsMgr::Update(float dt) {
        sb_assert(!m_lock_update);
        m_lock_update = true;
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
        float sdt = dt * m_speed;
		std::vector<ThreadPtr>::iterator i = m_threads.begin();
		while (i!=m_threads.end()) {
			if ((*i)->Update(sdt))
				i = m_threads.erase(i);
			else
				i++;
		}
        for (size_t i=0;i<m_removed_threads.size();i++) {
            std::vector<ThreadPtr>::iterator it = std::find(m_threads.begin(),m_threads.end(),m_removed_threads[i]);
            if (it!=m_threads.end())
                m_threads.erase(it);
        }
        m_removed_threads.clear();
        m_lock_update = false;
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
        if (!m_lock_update) {
            it = std::find(m_threads.begin(),m_threads.end(),t);
            if (it != m_threads.end()) {
                m_threads.erase(it);
            }
            return;
        }
		it = std::find(m_removed_threads.begin(),m_removed_threads.end(),t);
		if (it==m_removed_threads.end())
			m_removed_threads.push_back(t);
		
	}
	void ThreadsMgr::Clear() {
		m_added_threads.clear();
        if (!m_lock_update) {
            for (std::vector<ThreadPtr>::iterator it = m_threads.begin();it!=m_threads.end();++it) {
                (*it)->Clear();
            }
            m_threads.clear();
            m_removed_threads.clear();
            return;
        }
		for (size_t i=0;i<m_threads.size();i++) {
			std::vector<ThreadPtr>::iterator it = std::find(m_removed_threads.begin(),m_removed_threads.end(),m_threads[i]);
            if (it==m_removed_threads.end()) {
				m_removed_threads.push_back(m_threads[i]);
                m_threads[i]->Clear();
            }
		}
	}

    bool PermanentThreadsMgr::Update(float dt) {
        ThreadsMgr::Update(dt);
        return false;
    }
}
