

#include "tasks_pool.h"
#include <sbstd/sb_vector.h>
#include <algorithm>

#if 1
#include <pthread.h>
#include <unistd.h>


struct TasksPool::Impl {
    size_t m_max_tasks;
    bool m_finish;
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_cond;
    sb::vector<pthread_t> m_threads;
    tasks_list_t m_added_tasks;
    size_t m_worked_tasks;
    tasks_list_t m_completed_tasks;
    Impl(size_t tasks) : m_max_tasks(tasks),m_worked_tasks(0) {
        m_finish = false;
        pthread_mutex_init(&m_mutex, 0);
        pthread_cond_init(&m_cond, 0);
    }
    ~Impl() {
        lock();
        m_added_tasks.clear();
        m_completed_tasks.clear();
        m_finish = true;
        unlock();
    
        while (true) {
            
            lock();
            if (m_threads.empty()) {
                unlock();
                break;
            }
            pthread_cond_broadcast(&m_cond);
            unlock();
            usleep(100);
        }
        
        
        pthread_cond_destroy(&m_cond);
        pthread_mutex_destroy(&m_mutex);
    }
    void lock() {
        pthread_mutex_lock(&m_mutex);
    }
    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }
    void add(const TaskPtr& t) {
        lock();
        m_added_tasks.push_back(t);
        if (m_added_tasks.size() > m_threads.size() &&
            m_threads.size() < m_max_tasks) {
            run_new_thread();
        }
        pthread_cond_signal(&m_cond);
        unlock();
    }
    static void* thread_func(void* d) {
        Impl* self_ = static_cast<Impl*>(d);
        bool complete = false;
        while (!complete) {
            TaskPtr t;
            {
                self_->lock();
                while (!self_->m_finish) {
                    if (!self_->m_added_tasks.empty()) {
                        t = self_->m_added_tasks.front();
                        self_->m_added_tasks.pop_front();
                        self_->m_worked_tasks++;
                        break;
                    }
                    pthread_cond_wait(&self_->m_cond, &self_->m_mutex);
                }
                complete = self_->m_finish;
                self_->unlock();
            }
            if (t && !complete) {
                t->Run();
                {
                    self_->lock();
                    self_->m_worked_tasks--;
                    self_->m_completed_tasks.push_back(t);
                    t.reset();
                    self_->unlock();
                }
            }
        }
        self_->lock();
        self_->m_threads.erase(std::remove(self_->m_threads.begin(),self_->m_threads.end(),pthread_self()),self_->m_threads.end());
        self_->unlock();
        return 0;
    }
    void run_new_thread() {
        pthread_t thread;
        pthread_create(&thread, 0, &thread_func, this);
        m_threads.push_back(thread);
    }
    bool process() {
        bool error = false;
        lock();
        for (tasks_list_t::iterator it = m_completed_tasks.begin();it!=m_completed_tasks.end();++it) {
            if (!(*it)->OnComplete()) {
                error = true;
            }
        }
        m_completed_tasks.clear();
        unlock();
        return !error;
    }
    
    bool is_completed() {
        bool res = m_completed_tasks.empty();
        lock();
        res = res && m_added_tasks.empty() && (m_worked_tasks == 0);
        unlock();
        return res;
    }
    
};



#endif

TasksPool::TasksPool(size_t tasks) : m_impl(new Impl(tasks)) {
    
}
TasksPool::~TasksPool() {
    delete m_impl;
}
void TasksPool::AddTask(const TaskPtr& t) {
    m_impl->add(t);
}
bool TasksPool::Process() {
    return m_impl->process();
}
bool TasksPool::Completed() {
    return m_impl->is_completed();
}
