#include <simcode/net/EventChannel.h>

#ifndef WIN32
#include <simcode/net/CommonHead.h>
#endif
using namespace simcode;
using namespace net;

#ifndef WIN32
int EventChannel::POLL::ZERO = 0;
int EventChannel::POLL::READ = EPOLLIN;
int EventChannel::POLL::WRITE = EPOLLOUT;
int EventChannel::POLL::PRI = EPOLLPRI;
int EventChannel::POLL::ERR = EPOLLERR;
#else

int EventChannel::POLL::ZERO = 0;
int EventChannel::POLL::READ = 0x001;
int EventChannel::POLL::WRITE = 0x004;
int EventChannel::POLL::PRI = 0x002;
int EventChannel::POLL::ERR = 0x008;
#endif

EventChannel::EventChannel(EventLoop* loop__, int fd__, const EventCallback& b):
    loop_(loop__),
    fd_(fd__),
    events_(POLL::ZERO),
    revents_(POLL::ZERO),
	eventCallback_(b)
{
}

EventChannel::~EventChannel()
{
	
}

void EventChannel::update()
{
	loop_->modifyChannel(shared_from_this());
}

void EventChannel::handleEvent(int revents__)
{
	revents_ = revents__;
	eventCallback_(this);
	clearRevent();
}

