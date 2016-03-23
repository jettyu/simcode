#include <simcode/net/Timer.h>
#include <assert.h>
#include <sys/timerfd.h>
#include <sys/time.h>
#include <strings.h>
#include <unistd.h>
using namespace simcode;
using namespace net;

const int kMicroSecondsPerSecond = 1000*1000;

static uint64_t getCurMicrosecod()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return seconds * kMicroSecondsPerSecond + tv.tv_usec;
}

Timer::Timer(const RemoveFdCallback& b):
    timerfd_(timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC)),
    removeCallback_(b)
{
    assert(timerfd_ != -1);
    bzero(&handler_, sizeof(handler_));
}

Timer::~Timer()
{
    close(timerfd_);
}

void Timer::handleEvent(uint32_t events)
{
    handler_.eventCallback();
    uint64_t s;
    read(timerfd_, &s, sizeof(s));
    if (handler_.intervalTime == 0 || //没有间隔时间，只执行1次
            handler_.surplusTimes == 1 //达到了最大次数
       )
    {
        if (removeCallback_) removeCallback_(timerfd_);
        return;
    }
    if (handler_.surplusTimes != 0) handler_.surplusTimes--;
}

void Timer::setTimer(const EventCallback& b, double afterTime, double intervalTime, int maxTimes)
{
    int now = time(NULL);
    handler_.eventCallback = b;
    handler_.surplusTimes = maxTimes;
    handler_.intervalTime = intervalTime;
    struct timespec curValue;
    struct itimerspec newValue;
    bzero(&curValue, sizeof(curValue));
    bzero(&newValue, sizeof(newValue));
    clock_gettime(CLOCK_REALTIME, &curValue);
    newValue.it_value.tv_sec = curValue.tv_sec + int(afterTime);
    newValue.it_value.tv_nsec = curValue.tv_nsec + int(double(double(afterTime-int(afterTime))*1000000000));
    newValue.it_value.tv_sec += newValue.it_value.tv_nsec / 1000000000;
    newValue.it_value.tv_nsec = newValue.it_value.tv_nsec % 1000000000;
    newValue.it_interval.tv_sec = int(intervalTime);
    newValue.it_interval.tv_nsec = int(double((double(intervalTime) - int(intervalTime))*1000000000));
    newValue.it_interval.tv_sec += newValue.it_interval.tv_nsec / 1000000000;
    newValue.it_interval.tv_nsec = newValue.it_interval.tv_nsec % 1000000000;
    printf("%f|%f\n", afterTime, intervalTime);
    printf("%ld|%ld|%ld|%ld\n", newValue.it_value.tv_sec, newValue.it_value.tv_nsec, newValue.it_interval.tv_sec, newValue.it_interval.tv_nsec);
    assert(timerfd_settime(timerfd_, TFD_TIMER_ABSTIME, &newValue, NULL)!= -1);
}

