#include <simcode/net/EventLoop.h>
using namespace simcode;
using namespace simcode::net;

void EventLoop::runInLoop(int id, const EPollPoller::EventCallback& b, int events)
{
    ScopeLock lock(mutex_);
    assert( 0 == poller_.addEvent(id, b, events));
}

void EventLoop::loop()
{
    while (1)
    {
        poller_.poll(10000);
    }
}

void EventLoop::runAfter(double afterTime, const Timer::EventCallback& c)
{
    TimerPtr timer(new Timer(simex::bind(&EventLoop::removeTimer, this, _1)));
    timer->setTimer(c, afterTime);
    ScopeLock lock(mutex_);
    poller_.addEvent(timer->timerfd(),
                     simex::bind(&Timer::handleEvent, get_pointer(timer), _1));
    timerList_[timer->timerfd()] = timer;
}

void EventLoop::runEvery(double intervalTime, const Timer::EventCallback& c)
{
    TimerPtr timer (new Timer(simex::bind(&EventLoop::removeTimer, this, _1)));
    timer->setTimer(c, intervalTime, intervalTime, 0);
    ScopeLock lock(mutex_);
    poller_.addEvent(timer->timerfd(),
                     simex::bind(&Timer::handleEvent, get_pointer(timer), _1));
    timerList_[timer->timerfd()] = timer;
}

void EventLoop::modifyEvent(int id, int events)
{
    ScopeLock lock(mutex_);
    poller_.modifyEvent(id, events);
}

void EventLoop::removeInLoop(int id)
{
    ScopeLock lock(mutex_);
    poller_.removeEvent(id);
}

void EventLoop::removeTimer(int id)
{
    removeInLoop(id);
    {
        ScopeLock lock(mutex_);
        timerList_.erase(id);
    }
}
