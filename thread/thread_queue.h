#ifndef _THREAD_QUEUE_H_
#define _THREAD_QUEUE_H_
#include <simcode/base/typedef.h>
#include <vector>
#include <deque>

namespace simcode
{
namespace thread
{
typedef SimFunction<void()> ThreadHandlerFunc;
class ThreadQueue
{
public:
    ThreadQueue();
    void start();
    void stop();
    int push_back(const ThreadHandlerFunc& func);

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
    std::deque<ThreadHandlerFunc> deq_;
    Mutex    mtx_;
    ConditionVar cond_;
    bool is_started_;
    size_t max_size_;
    size_t thread_count_;
    size_t size_;
};

typedef SharedPtr<ThreadQueue> ThreadQueuePtr;

}
}
#endif
