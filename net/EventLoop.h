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
    void runInLoop(int id, const EPollPoller::EventCallback& c, int events=EPOLLIN|EPOLLPRI);
    void loop();
    void runAfter(double afterTime, const Timer::EventCallback& c);
    void runEvery(double intervalTime, const Timer::EventCallback& c);
    void modifyEvent(int id, int events);
    void removeInLoop(int id);
private:
    void removeTimer(int id);
    simcode::net::EPollPoller poller_;
    typedef SharedPtr<Timer> TimerPtr;
    std::map<int, TimerPtr> timerList_;
    simex::mutex mutex_;
};
}
}

#endif // SIMCODE_EVENT_LOOP_H
