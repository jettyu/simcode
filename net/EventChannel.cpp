#include <simcode/net/EventChannel.h>

#ifndef WIN32
#include <simcode/net/CommonHead.h>
#endif
using namespace simcode;
using namespace net;

#ifndef WIN32
uint32_t EventChannel::POLL::ZERO = 0;
uint32_t EventChannel::POLL::READ = EPOLLIN;
uint32_t EventChannel::POLL::WRITE = EPOLLOUT;
uint32_t EventChannel::POLL::PRI = EPOLLPRI;
uint32_t EventChannel::POLL::ERR = EPOLLERR;
#else

uint32_t EventChannel::POLL::ZERO = 0;
uint32_t EventChannel::POLL::READ = 0x001;
uint32_t EventChannel::POLL::WRITE = 0x004;
uint32_t EventChannel::POLL::PRI = 0x002;
uint32_t EventChannel::POLL::ERR = 0x008;
#endif

EventChannel::EventChannel(EventLoop* loop__, int fd__, const EventCallback& b):
    loop_(loop__),
    fd_(fd__),
    events_(POLL::ZERO),
    revents_(POLL::ZERO),
	eventCallback_(b),
	tieFlag_(false),
	isRemoved_(false)
{
}

EventChannel::~EventChannel()
{
}

void EventChannel::update()
{
	loop_->modifyChannel(shared_from_this());
}

void EventChannel::handleEvent(uint32_t revents__)
{
	revents_ = revents__;
	simex::shared_ptr<void> tieObj;
	if (tieFlag_)
    {
        tieObj = tie_.lock();
        if (!tieObj)
        {
            removeInLoop();
            return;
        }

    }
	eventCallback_(this);
	clearRevent();
}

