#ifndef SIMCODE_EVENT_LOOP_H
#define SIMCODE_EVENT_LOOP_H
#include <simcode/net/EPollPoller.h>
#include <simcode/base/noncopyable.h>
#include <simcode/thread/thread_queue.h>
#include <simcode/net/Timer.h>
namespace simcode
{
namespace net
{
class EventLoop : noncopyable
{
public:
    typedef simex::function<void()> TaskCallback;
    EventLoop();
    ~EventLoop();
    void runInLoop(int id, const EPollPoller::EventCallback& c, int events=EPOLLIN|EPOLLPRI);
    void loop();
    void runAfter(double afterTime, const Timer::EventCallback& c);
    void runEvery(double intervalTime, const Timer::EventCallback& c);
    void modifyEvent(int id, int events);
    void removeInLoop(int id);
    void addTask(const TaskCallback& b);
private:
    void doTask();
    void wakeup();
    void wakeupHandler(int events);
private:
    void removeTimer(int id);
    simcode::net::EPollPoller poller_;
    typedef SharedPtr<Timer> TimerPtr;
    typedef std::map<int, TimerPtr> TimerList;
    TimerList timerList_;
    typedef std::vector<TaskCallback> TaskList;
    TaskList tasks_;
    simex::mutex mutex_;
    int wakeupfd_;
};
}
}

#endif // SIMCODE_EVENT_LOOP_H
