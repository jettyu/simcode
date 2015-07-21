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
    typedef::simex::shared_ptr<EventChannel> Ptr;
	typedef simex::function<void(EventChannel*)> EventCallback;
	EventChannel(EventLoop* loop__, int fd__, const EventCallback& b);
    ~EventChannel();

    int fd() const
    {
        return fd_;
    }
    uint32_t events() const
    {
        return events_;
    }
    void set_revents(uint32_t re)
    {
        revents_ = re;
    }
	uint32_t revents() const
	{
		return revents_;
	}
	void tie(const simex::shared_ptr<void> tie__)
	{
	    tie_ = tie__;
	    tieFlag_ = true;
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
	void handleEvent(uint32_t revents__);
private:

    void update();

private:
    struct POLL
    {
        static uint32_t ZERO;
        static uint32_t READ;
        static uint32_t WRITE;
        static uint32_t PRI;
		static uint32_t ERR;
    };
private:
    EventLoop* loop_;
    int fd_;
    uint32_t events_;
    uint32_t revents_;
	EventCallback eventCallback_;
	simex::weak_ptr<void> tie_; //∑¿÷πeventCallback ß–ß
	bool tieFlag_;
};

}
}

#endif // SIMCODE_NET_EVENT_CHANNEL_H
