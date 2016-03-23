#include <simcode/net/EventLoopThreadPool.h>
using namespace simcode;
using namespace net;

EventLoop* EventLoopThreadPool::getNextLoop()
{
    if (loopPool_.empty()) return NULL;
    if (next_ >= threadNum_) next_ = 0;
    return loopPool_[next_++];
}

void EventLoopThreadPool::start()
{
    for (int i=0; i<threadNum_; ++i)
    {
        EventLoopThreadPtr e(new EventLoopThread);
        loopThreadPool_.push_back(e);
        loopPool_.push_back(e->startLoop());
    }
}
