#include <simcode/net/EventLoop.h>
#include <simcode/net/EventChannel.h>
using namespace simcode;
using namespace net;

EventLoop::EventLoop(void)
{
	curThreadId_ = simex::this_thread::get_id();
}


EventLoop::~EventLoop(void)
{

}

void EventLoop::addChannel(const simex::shared_ptr<EventChannel>& c)
{
	if (inOneThread()) 
		selector_.addChannel(c);
	else
		addTask(simex::bind(&Selector::addChannel, &selector_, c));
}

void EventLoop::removeChannel(int fd)
{
	if (inOneThread()) 
		selector_.removeChannel(fd);
	else
		addTask(simex::bind(&Selector::removeChannel, &selector_, fd));
}

void EventLoop::modifyChannel(const simex::shared_ptr<EventChannel>& c)
{
	if (inOneThread()) 
		selector_.modifyChannel(c);
	else
		addTask(simex::bind(&Selector::modifyChannel, &selector_, c));
}

void EventLoop::addTask(const TaskCallback& b)
{
	ScopeLock lock(mutex_);
	if (tasks_.empty()) wakeup();
	tasks_.push_back(b);
}

void EventLoop::doTask()
{
	TaskList tmpTaskList;
	{
	ScopeLock lock(mutex_);
	tmpTaskList.swap(tasks_);
	}
	TaskList::iterator it;
	for (it=tmpTaskList.begin(); it!=tmpTaskList.end(); ++it)
		(*it)();
}

void EventLoop::loop()
{
	curThreadId_ = simex::this_thread::get_id();
	while (true)
	{
		selector_.poll(100000, 0);
		doTask();
	}
}

void EventLoop::wakeup()
{

}

void EventLoop::wakeupHandle(EventChannel* c)
{

}