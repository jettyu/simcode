#include <simcode/net/EventLoop.h>
#include <simcode/base/logger.h>
using namespace simcode;
using namespace simcode::net;

EventLoop::EventLoop() :
    wakeupfd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC))
{
    poller_.addEvent(wakeupfd_, simex::bind(&EventLoop::wakeupHandler, this, _1));
}

EventLoop::~EventLoop()
{
    ::close(wakeupfd_);
}

void EventLoop::runInLoop(int id, const EPollPoller::EventCallback& b, int events)
{
    addTask(simex::bind(&EPollPoller::addEvent, &poller_, id, b, events));
    //assert( 0 == poller_.addEvent(id, b, events));
}

void EventLoop::loop()
{
    while (1)
    {
        poller_.poll(10000);
        doTask();
    }
}

void EventLoop::runAfter(double afterTime, const Timer::EventCallback& c)
{
    TimerPtr timer(new Timer(simex::bind(&EventLoop::removeTimer, this, _1)));
    timer->setTimer(c, afterTime);
//    ScopeLock lock(mutex_);
//    poller_.addEvent(timer->timerfd(),
//                     simex::bind(&Timer::handleEvent, get_pointer(timer), _1));
    runInLoop(timer->timerfd(), simex::bind(&Timer::handleEvent, get_pointer(timer), _1));
    timerList_[timer->timerfd()] = timer;
}

void EventLoop::runEvery(double intervalTime, const Timer::EventCallback& c)
{
    TimerPtr timer (new Timer(simex::bind(&EventLoop::removeTimer, this, _1)));
    timer->setTimer(c, intervalTime, intervalTime, 0);
//    ScopeLock lock(mutex_);
//    poller_.addEvent(timer->timerfd(),
//                     simex::bind(&Timer::handleEvent, get_pointer(timer), _1));
    runInLoop(timer->timerfd(), simex::bind(&Timer::handleEvent, get_pointer(timer), _1));
    timerList_[timer->timerfd()] = timer;
}

void EventLoop::modifyEvent(int id, int events)
{
    addTask(simex::bind(&EPollPoller::modifyEvent, &poller_, id, events));
}

void EventLoop::removeInLoop(int id)
{
    addTask(simex::bind(&EPollPoller::removeEvent, &poller_, id));
}

void EventLoop::removeTimer(int id)
{
    removeInLoop(id);
    {
        ScopeLock lock(mutex_);
        timerList_.erase(id);
    }
}

void EventLoop::addTask(const TaskCallback& b)
{
    ScopeLock lock(mutex_);
    tasks_.push_back(b);
    wakeup();
}

void EventLoop::doTask()
{
    TaskList tmpTasks;
    {
        ScopeLock lock(mutex_);
        tmpTasks.swap(tasks_);
    }
    for (TaskList::iterator it=tmpTasks.begin(); it!=tmpTasks.end(); ++it)
        (*it)();
}

void EventLoop::wakeup()
{
    uint64_t i = 1;
    int n = ::write(wakeupfd_, &i, sizeof(i));
    if (n != sizeof(i)) LOG_ERROR("n=%d\n", n);
}

void EventLoop::wakeupHandler(int events)
{
    uint64_t i = 0;
    int n =::read(wakeupfd_, &i, sizeof(i));
    if (n != sizeof(i)) LOG_ERROR("n=%d\n", n);
}


