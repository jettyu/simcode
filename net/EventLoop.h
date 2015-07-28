#ifndef SIMCODE_EVENT_LOOP_H
#define SIMCODE_EVENT_LOOP_H

#include <simcode/base/noncopyable.h>
#include <simcode/thread/thread_queue.h>
#include <simcode/net/Timer.h>
#include <simcode/net/ChannelPoll.h>
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
    void runInLoop(const EventChannelPtr& c);
    void removeInLoop(int id);
    void modifyChannel(const EventChannelPtr& c);

    void loop();
    void runAfter(double afterTime, const Timer::EventCallback& c);
    void runEvery(double intervalTime, const Timer::EventCallback& c);

    void addTask(const TaskCallback& b);
	void execInLoop(const TaskCallback& b); //no relation of time seq, if inOneThread, do at soon, else addTask
    bool inOneThread() const
    {
        return curThreadId_ == simex::this_thread::get_id();
    }
private:
    void doTask();
    void wakeup();
    void wakeupHandler(EventChannel*);
    void timerHandler(EventChannel*, const simex::shared_ptr<Timer>& timer);
private:
    void removeTimer(int id);
    ChannelPoll poller_;
    typedef SharedPtr<Timer> TimerPtr;
    typedef std::map<int, TimerPtr> TimerList;
    TimerList timerList_;
    typedef std::vector<TaskCallback> TaskList;
    TaskList tasks_;
    simex::mutex mutex_;
    EventChannelPtr wakeupChannel_;
    bool isWakeuped_;
    simex::thread::id curThreadId_;
};
}
}

#endif // SIMCODE_EVENT_LOOP_H
