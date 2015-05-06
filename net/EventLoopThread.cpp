#include <simcode/net/EventLoopThread.h>

using namespace simcode;
using namespace simcode::net;

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb) : 
    callback_(cb)
{
}

EventLoop *EventLoopThread::startLoop()
{
    if (!loop_)
    {
        loop_.reset(new EventLoop);
        if (callback_) callback_(get_pointer(loop_));
        thread_.reset(new SimThread(SimBind(&EventLoop::loop, get_pointer(loop_))));
    }
    return get_pointer(loop_);
}


