#include <simcode/base/typedef.h>
#include <algorithm>
#include <simcode/thread/thread_queue_ex.h>
using namespace simcode;
using namespace simcode::thread;

ThreadQueueEx::ThreadQueueEx()
    :is_started_(false), max_size_(0), thread_count_(0),size_(0)
{
}
ThreadQueueEx::~ThreadQueueEx()
{
    stop();
}
void ThreadQueueEx::start()
{
    if (!is_started_)
    {
        is_started_ = true;
        work_tasks_.reserve(thread_count_);
        for (size_t i=0; i<thread_count_; ++i)
        {
            SimThreadPtr t(new SimThread(SimBind(&ThreadQueueEx::WorkTaskFunc, this)));
            work_tasks_.push_back(t);
        }
    }
}


void ThreadQueueEx::stop()
{
    if (!is_started_)
        return ;
    is_started_ = false;
    cond_.notify_all();
    std::for_each(work_tasks_.begin(), work_tasks_.end(),
                  simex::bind(&SimThread::join, _1));
}

int ThreadQueueEx::push_back(const TaskCallback& bp)
{
    ScopeLock lock(mtx_);
    if (size_ < max_size_ && ++size_)
        deq_.push_back(bp);
    else
        return 1;
    cond_.notify_one();
    return 0;
}

void ThreadQueueEx::WorkTaskFunc(void)
{
    simex::any context;
    ScopeLock lock(mtx_);
    while (is_started_)
    {
        while(!deq_.empty())
        {
            TaskCallback e = deq_.front();
            deq_.pop_front();
            --size_;
            lock.unlock();
            e(&context);
            lock.lock();
        }
        cond_.wait(lock);
    }
}
