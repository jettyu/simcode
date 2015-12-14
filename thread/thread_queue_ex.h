#ifndef _THREAD_QUEUE_EX_H_
#define _THREAD_QUEUE_EX_H_
#include <vector>
#include <deque>
#include <simcode/base/typedef.h>

namespace simcode
{
namespace thread
{
class ThreadQueueEx
{
public:
    typedef SimFunction<void(simex::any *context)> TaskCallback;
    ThreadQueueEx();
    virtual ~ThreadQueueEx();
    void start();
    void stop();
    int push_back(const TaskCallback& func);

    inline void set_maxsize(size_t n = 1024)
    {
        max_size_ = n;
    }
    inline void set_threadcount(size_t n)
    {
        thread_count_ = n;
    }
    inline size_t max_size()
    {
        return max_size_;
    }
    inline size_t threadcnt()
    {
        return thread_count_;
    }
private:
    void WorkTaskFunc(void);
private:

    std::vector<SimThreadPtr> work_tasks_;
    std::deque<TaskCallback> deq_;
    Mutex    mtx_;
    ConditionVar cond_;
    bool is_started_;
    size_t max_size_;
    size_t thread_count_;
    size_t size_;
};

typedef SharedPtr<ThreadQueueEx> ThreadQueueExPtr;

}
}
#endif
