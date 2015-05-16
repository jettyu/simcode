#ifndef SIMCODE_NET_TIMER_H
#define SIMCODE_NET_TIMER_H
#include <simcode/base/noncopyable.h>
#include <simcode/base/typedef.h>
namespace simcode
{
namespace net
{

class Timer :  noncopyable
{
public:
    typedef simex::function<void()> EventCallback;
    typedef simex::function<void(int fd)> RemoveFdCallback;
private:
    struct TimerHandler
    {
        int surplusTimes;
        double intervalTime;
        EventCallback eventCallback;
    };
public:
    Timer(const RemoveFdCallback& b=NULL);
    ~Timer();
    void handleEvent(int events);
    void setTimer(const EventCallback& b,
                  double afterTime,              //多少时间后开始执行
                  double intervalTime = 0,       //每隔多久时间执行1次
                  int maxTimes = 1);      //最多执行多少次,0表示不限次数
    int timerfd() const
    {
        return timerfd_;
    }
private:
    int timerfd_;
    TimerHandler handler_;
    RemoveFdCallback removeCallback_;
};
}
}

#endif
