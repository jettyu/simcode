#pragma once

#include <simcode/net/Socket.h>
#include <simcode/net/Selector.h>
#include <simcode/base/typedef.h>
#include <vector>
namespace simcode
{
namespace net
{
class EventLoop
{
public:
	typedef simex::function<void()> TaskCallback;
	EventLoop(void);
	~EventLoop(void);
	void addChannel(const simex::shared_ptr<EventChannel>& c);
	void removeChannel(int fd);
	void modifyChannel(const simex::shared_ptr<EventChannel>& c);
	void addTask(const TaskCallback& b);
	void loop();
	
private:
	void doTask();
	void wakeup();
	void wakeupHandle(EventChannel*);
private:
	Selector selector_;
	Mutex mutex_;
	typedef std::vector<TaskCallback> TaskList;
	TaskList tasks_;
};

}
}