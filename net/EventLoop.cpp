#include <simcode/net/EventLoop.h>
#include <simcode/base/logger.h>
#include <simcode/net/EventChannel.h>
using namespace simcode;
using namespace simcode::net;

EventLoop::EventLoop() :
    isWakeuped_(false),
    closed_(0)
{
    curThreadId_ = simex::this_thread::get_id();
    int wakeupfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    wakeupChannel_.reset(new EventChannel(this, wakeupfd, simex::bind(&EventLoop::wakeupHandler, this, _1)));
    wakeupChannel_->enableReading();
    poller_.addChannel(wakeupChannel_);
}

EventLoop::~EventLoop()
{
    ::close(wakeupChannel_->fd());
}

void EventLoop::runInLoop(const EventChannelPtr& c)
{
    if (inOneThread())
        poller_.addChannel(c);
    else
        addTask(simex::bind(&ChannelPoll::addChannel, &poller_, c));
    //assert( 0 == poller_.addEvent(id, b, events));
}

void EventLoop::loop()
{
    curThreadId_ = simex::this_thread::get_id();
    while (!isClosed())
    {
        poller_.poll(10000);
        doTask();
    }
}

void EventLoop::runAfter(double afterTime, const Timer::EventCallback& c)
{
    TimerPtr timer(new Timer(simex::bind(&EventLoop::removeTimer, this, _1)));
    timer->setTimer(c, afterTime);
    EventChannelPtr ec(new EventChannel(this, timer->timerfd(), simex::bind(&EventLoop::timerHandler,
                                        this,
                                        _1,
                                        timer
                                                                           )));
    ec->tie(timer);
    ec->enableReading();
    runInLoop(ec);
    timerList_[timer->timerfd()] = timer;
}

void EventLoop::runEvery(double intervalTime, const Timer::EventCallback& c)
{
    TimerPtr timer (new Timer(simex::bind(&EventLoop::removeTimer, this, _1)));
    timer->setTimer(c, intervalTime, intervalTime, 0);
    EventChannelPtr ec(new EventChannel(this, timer->timerfd(), simex::bind(&EventLoop::timerHandler,
                                        this,
                                        _1,
                                        timer
                                                                           )));
    ec->tie(timer);
    ec->enableReading();
    runInLoop(ec);
    timerList_[timer->timerfd()] = timer;
}

void EventLoop::modifyChannel(const EventChannelPtr& ec)
{
    if (inOneThread())
        poller_.modifyChannel(ec);
    else
        addTask(simex::bind(&ChannelPoll::modifyChannel, &poller_, ec));
}

void EventLoop::removeInLoop(int id)
{
    if (inOneThread())
        poller_.removeChannel(id);
    else
        addTask(simex::bind(&ChannelPoll::removeChannel, &poller_, id));
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
    if (inOneThread())
    {
        b();
        return;
    }
    ScopeLock lock(mutex_);
    tasks_.push_back(b);
    if (!isWakeuped_) wakeup();
}

void EventLoop::execInLoop(const TaskCallback& b)
{
	if (inOneThread())
        b();
	else
		addTask(b);
}

void EventLoop::close()
{
    addTask(simex::bind(&EventLoop::setClose, this));
}

void EventLoop::doTask()
{
    TaskList tmpTasks;
    {
        ScopeLock lock(mutex_);
        tmpTasks.swap(tasks_);
        isWakeuped_ = false;
    }
    for (TaskList::iterator it=tmpTasks.begin(); it!=tmpTasks.end(); ++it)
        (*it)();
}

void EventLoop::wakeup()
{
    isWakeuped_ = true;
    uint64_t i = 1;
    int n = ::write(wakeupChannel_->fd(), &i, sizeof(i));
    if (n != sizeof(i)) LOG_ERROR("n=%d\n", n);
}

void EventLoop::wakeupHandler(EventChannel*)
{
    uint64_t i = 0;
    int n =::read(wakeupChannel_->fd(), &i, sizeof(i));
    if (n != sizeof(i)) LOG_ERROR("n=%d\n", n);
}

void EventLoop::timerHandler(EventChannel* ec, const simex::shared_ptr<Timer>& timer)
{
    timer->handleEvent(ec->revents());
}


