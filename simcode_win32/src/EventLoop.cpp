#include <simcode/net/EventLoop.h>

using namespace simcode;
using namespace net;

EventLoop::EventLoop(void)
{
}


EventLoop::~EventLoop(void)
{
}

void EventLoop::addChannel(const simex::shared_ptr<Channel>& c)
{
	addTask(simex::bind(&Selector::addChannel, &selector_, c));
}

void EventLoop::removeChannel(int fd)
{
	addTask(simex::bind(&Selector::removeChannel, &selector_, fd));
}

void EventLoop::addTask(const TaskCallback& b)
{
	ScopeLock lock(mutex_);
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
	while (true)
	{
		selector_.poll(100000, 0);
		doTask();
	}
}