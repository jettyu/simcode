#ifndef SIMCODE_NET_EVENT_CHANNEL_H
#define SIMCODE_NET_EVENT_CHANNEL_H

#include <simcode/net/EventLoop.h>
#include <simcode/base/typedef.h>
namespace simcode
{
namespace net
{

class EventChannel : noncopyable
{
public:
    typedef simex::function<void(EventChannel*)> EventCallback;
    EventChannel(EventLoop* loop__, int fd__);
    ~EventChannel();
    int fd() const
    {
        return fd_;
    }
    int events() const
    {
        return events_;
    }
    void set_revents(int re)
    {
        revents_ = re;
    }
    EventLoop* getLoop()
    {
        return loop_;
    }
    void runInLoop();
    void Close();
    void setEventCallback(const EventCallback& c)
    {
        eventCallback_ = c;
    }

    bool isNoneEvent() const
    {
        return events_ == POLL::ZERO;
    }

    void enableReading()
    {
        events_ |= POLL::IN | POLL::PRI;
        update();
    }
    void disableReading()
    {
        events_ &= ~POLL::IN;
        update();
    }
    void enableWriting()
    {
        events_ |= POLL::OUT;
        update();
    }
    void disableWriting()
    {
        events_ &= ~POLL::OUT;
        update();
    }
    void disableAll()
    {
        events_ = POLL::ZERO;
        update();
    }
    bool isWriting() const
    {
        return events_ & POLL::OUT;
    }
    bool isReading() const
    {
        return events_ & POLL::IN;
    }
private:
    void handleEvent(int revents__);
    void update();
private:
    struct POLL
    {
        static int ZERO;
        static int IN;
        static int OUT;
        static int PRI;
    };
private:
    EventLoop* loop_;
    int fd_;
    int events_;
    int revents_;

    EventCallback eventCallback_;
};

}
}

#endif // SIMCODE_NET_EVENT_CHANNEL_H
