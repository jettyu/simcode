#include <simcode/net/EventLoop.h>
#include <simcode/net/EventChannel.h>
using namespace simcode;
using namespace net;

EventLoop::EventLoop(void)
{
#if 0
	const int SIP_UDP_CONNRESET=-1744830452;
	int wakeupfd = ::socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	::bind(wakeupfd, addr_.addr(), sizeof(sockaddr));
	printf("wakeupfd=%d\n", wakeupfd);
	wakeupChannel_.reset(new EventChannel(this, wakeupfd, simex::bind(&EventLoop::wakeupHandle, this, _1)));
	wakeupChannel_->enableReading();  
#endif // 0

//	selector_.addChannel(wakeupChannel_);
}


EventLoop::~EventLoop(void)
{
#if 0
	::closesocket(wakeupChannel_->fd());  
#endif // 0

}

void EventLoop::addChannel(const simex::shared_ptr<EventChannel>& c)
{
	addTask(simex::bind(&Selector::addChannel, &selector_, c));
}

void EventLoop::removeChannel(int fd)
{
	addTask(simex::bind(&Selector::removeChannel, &selector_, fd));
}

void EventLoop::modifyChannel(const simex::shared_ptr<EventChannel>& c)
{
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
	while (true)
	{
		selector_.poll(100000, 0);
		doTask();
	}
}

void EventLoop::wakeup()
{
#if 0
	uint64_t i = 1;
	int ret = ::sendto(wakeupChannel_->fd(), reinterpret_cast<const char*>(&i), sizeof(i), 0, addr_.addr(), sizeof(sockaddr));
	printf("ret=%d\n", ret);  
#endif // 0

}

void EventLoop::wakeupHandle(EventChannel* c)
{
#if 0
	uint64_t i = 0;
	sockaddr addr = {0};
	int addrlen = sizeof(sockaddr);
	int ret = ::recvfrom(wakeupChannel_->fd(), reinterpret_cast<char*>(&i), sizeof(i), 0, &addr, &addrlen);
	printf("ret=%d\n", ret);
	printf("errcode=%d\n", WSAGetLastError());  
#endif // 0

}