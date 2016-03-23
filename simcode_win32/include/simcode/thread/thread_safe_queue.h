#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H
#include <simcode/thread/thread_queue.h>

namespace simcode
{
namespace thread
{
class ThreadSafeQueue
{
public:
    ThreadSafeQueue();
    ~ThreadSafeQueue();
    void start();
    void stop();
    int push_back(size_t id, const ThreadHandlerFunc& h);
    void setThreadNum(int n)
    {
        threadNum_ = n;
    }
    void setMaxQueueSize(int n)
    {
        maxQueueSize_ = n;
    }
private:
    std::vector<ThreadQueuePtr> queues_;
    int threadNum_;
    int maxQueueSize_;
};

typedef SharedPtr<ThreadSafeQueue> ThreadSafeQueuePtr;
}
}
#endif
