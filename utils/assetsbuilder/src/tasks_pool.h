#ifndef _TASKS_POOL_H_INCLUDED_
#define _TASKS_POOL_H_INCLUDED_

#include <sbstd/sb_intrusive_ptr.h>
#include <sbstd/sb_list.h>

class Task : public sb::ref_countered_base_not_copyable {
private:
    bool m_error;
    virtual bool OnCompleteImpl() { return true; }
    virtual bool RunImpl() = 0;
public:
    Task() : m_error(false) {}
    void Run() {
        if (!RunImpl()) {
            m_error = true;
        }
    }
    bool OnComplete() {
        if (m_error) return false;
        return OnCompleteImpl();
    }
    
};
typedef sb::intrusive_ptr<Task> TaskPtr;

class TasksPool {
private:
    typedef sb::list<TaskPtr> tasks_list_t;
    
    struct Impl;
    Impl* m_impl;
public:
    TasksPool(size_t tasks);
    ~TasksPool();
    void AddTask(const TaskPtr& t);
    bool Process();
    bool Completed();
};

#endif /* _TASKS_POOL_H_INCLUDED_ */
