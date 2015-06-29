#include <simcode/thread/thread_queue.h>
#include <algorithm>
using namespace simcode;
using namespace simcode::thread;

ThreadQueue::ThreadQueue()
    :is_started_(false), max_size_(0), thread_count_(0),size_(0)
{
}
ThreadQueue::~ThreadQueue()
{
    stop();
}
void ThreadQueue::start()
{
    if (!is_started_)
    {
        is_started_ = true;
        work_tasks_.reserve(thread_count_);
        for (size_t i=0; i<thread_count_; ++i)
        {
            SimThreadPtr t(new SimThread(SimBind(&ThreadQueue::WorkTaskFunc, this)));
            work_tasks_.push_back(t);
        }
    }
}

void ThreadQueue::stop()
{
    if (!is_started_)
        return ;
    is_started_ = false;
    cond_.notify_all();
    std::for_each(work_tasks_.begin(), work_tasks_.end(),
                  boost::bind(&SimThread::join, _1));
}

int ThreadQueue::push_back(const ThreadHandlerFunc& bp)
{
    ScopeLock lock(mtx_);
    if (size_ < max_size_ && ++size_)
        deq_.push_back(bp);
    else
        return 1;
    cond_.notify_one();
    return 0;
}

void ThreadQueue::WorkTaskFunc(void)
{
    ScopeLock lock(mtx_);
    while (is_started_)
    {
        while(!deq_.empty())
        {
            ThreadHandlerFunc e = deq_.front();
            deq_.pop_front();
            --size_;
            lock.unlock();
            e();
            lock.lock();
        }
        cond_.wait(lock);
    }
}
