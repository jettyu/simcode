#include <simcode/thread/thread_safe_queue.h>
using namespace simcode;
using namespace simcode::thread;
ThreadSafeQueue::ThreadSafeQueue()
{
    threadNum_ = 1;
    maxQueueSize_ = 1024;
}

ThreadSafeQueue::~ThreadSafeQueue()
{
    stop();
}

void ThreadSafeQueue::start()
{
    queues_.reserve(threadNum_);
    for (int i=0; i<threadNum_; ++i)
    {
        ThreadQueuePtr tq(new ThreadQueue());
        tq->set_maxsize(maxQueueSize_);
        tq->set_threadcount(1);
        queues_.push_back(tq);
    }
    std::vector<ThreadQueuePtr>::iterator it;
    for (it=queues_.begin(); it!=queues_.end(); ++it)
        (*it)->start();
}

void ThreadSafeQueue::stop()
{
    std::vector<ThreadQueuePtr>::iterator it;
    for (it=queues_.begin(); it!=queues_.end(); ++it)
        (*it)->stop();
}

int ThreadSafeQueue::push_back(size_t id, const ThreadHandlerFunc& h)
{
    return queues_[id%threadNum_]->push_back(h);
}


