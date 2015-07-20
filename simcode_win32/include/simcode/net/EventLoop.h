#pragma once

#include <simcode/net/Selector.h>
#include <simcode/net/Channel.h>
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
	void addChannel(const simex::shared_ptr<Channel>& c);
	void removeChannel(int fd);
	void addTask(const TaskCallback& b);
	void loop();
private:
	void doTask();
private:
	Selector selector_;
	Mutex mutex_;
	typedef std::vector<TaskCallback> TaskList;
	TaskList tasks_;
};

}
}