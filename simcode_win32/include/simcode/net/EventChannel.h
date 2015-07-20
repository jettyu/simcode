#ifndef SIMCODE_NET_EVENT_CHANNEL_H
#define SIMCODE_NET_EVENT_CHANNEL_H

#include <simcode/net/EventLoop.h>
#include <simcode/base/typedef.h>
namespace simcode
{
namespace net
{

class EventChannel : noncopyable, 
	public simex::enable_shared_from_this<EventChannel>
{
public:
	typedef simex::function<void(EventChannel*)> EventCallback;
	EventChannel(EventLoop* loop__, int fd__, const EventCallback& b);
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
	int revents() const
	{
		return revents_;
	}
    EventLoop* getLoop()
    {
        return loop_;
    }

    void enableReading()
    {
        events_ |= POLL::READ | POLL::PRI;
        update();
    }
    void disableReading()
    {
        events_ &= ~POLL::READ;
        update();
    }
	bool isEnableReading() const
	{
		return (events_ & POLL::READ) != 0;
	}
    void enableWriting()
    {
        events_ |= POLL::WRITE;
        update();
    }
    void disableWriting()
    {
        events_ &= ~POLL::WRITE;
        update();
    }
	bool isEnableWriting() const
	{
		return (events_ & POLL::WRITE) != 0;
	}
    void disableAll()
    {
        events_ = POLL::ZERO;
        update();
    }


	void clearRevent()
	{
		revents_ = POLL::ZERO;
	}
	void setReventReading()
	{
		revents_ |= POLL::READ;
	}
	void setReventWriting()
	{
		revents_ |= POLL::WRITE;
	}
	void setReventError()
	{
		revents_ |= POLL::ERR;
	}
    bool isNoneEvent() const
    {
        return revents_ == POLL::ZERO;
    }
    bool isWriting() const
    {
        return (revents_ & POLL::WRITE) != 0;
    }
    bool isReading() const
    {
        return (revents_ & POLL::READ) != 0;
    }
	bool isError() const
	{
		return (revents_ & POLL::ERR) != 0;
	}
	void handleEvent(int revents__);
private:
    
    void update();
	
private:
    struct POLL
    {
        static int ZERO;
        static int READ;
        static int WRITE;
        static int PRI;
		static int ERR;
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
