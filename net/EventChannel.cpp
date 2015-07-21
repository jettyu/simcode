#include <simcode/net/EventChannel.h>
#include <simcode/net/CommonHead.h>

using namespace simcode;
using namespace net;

int EventChannel::POLL::ZERO = 0;
int EventChannel::POLL::IN = EPOLLIN;
int EventChannel::POLL::OUT = EPOLLOUT;
int EventChannel::POLL::PRI = EPOLLPRI;

EventChannel::EventChannel(EventLoop* loop__, int fd__):
    loop_(loop__),
    fd_(fd__),
    events_(0),
    revents_(0),
    isClosed_(false)
{
}

EventChannel::~EventChannel()
{
    close();
}

void EventChannel::runInLoop()
{
    loop_->runInLoop(fd_, simex::bind(&EventChannel::handleEvent, this, _1), events_);
}

void EventChannel::close()
{
    if (isClosed_) return;
    loop_->removeInLoop(fd_);
}

void EventChannel::update()
{
    loop_->modifyEvent(fd_, events_);
}

void EventChannel::handleEvent(int revents__)
{
    revents_ = revents__;
    if (eventCallback_) eventCallback_(this);
}
