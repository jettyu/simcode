#include <simcode/net/EventLoopThreadPool.h>
using namespace simcode;
using namespace net;

EventLoop* EventLoopThreadPool::getNextLoop()
{
    if (loopPool_.empty()) return NULL;
	return loopPool_[next_++%threadNum_];
}

void EventLoopThreadPool::start()
{
    for (uint16_t i=0; i<threadNum_; ++i)
    {
        EventLoopThreadPtr e(new EventLoopThread);
        loopThreadPool_.push_back(e);
        loopPool_.push_back(e->startLoop());
    }
}
