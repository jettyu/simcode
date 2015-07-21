#include <simcode/net/ChannelPoll.h>
#include <simcode/net/CommonHead.h>
#include <simcode/net/EventChannel.h>
#include <simcode/base/logger.h>

using namespace simcode;
using namespace simcode::net;
ChannelPoll::ChannelPoll():
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
    events_(kInitEventListSize)
{
    //ctor
}

ChannelPoll::~ChannelPoll()
{
    //dtor
    ::close(epollfd_);
}

int ChannelPoll::poll(int timeoutMs)
{
    int numEvents = ::epoll_wait(epollfd_,
                                 &events_[0],
                                 static_cast<int>(events_.size()),
                                 timeoutMs);
    int savedErrno = errno;
    if (numEvents > 0)
    {
        for (int i=0; i<numEvents; ++i)
        {
            reinterpret_cast<EventChannel*>(events_[i].data.ptr)->handleEvent(events_[i].events);
            //       int tfd = events_[i].data.fd;
            //       EventCallbackMap::iterator it;
            //       it = eventCallbackMap_.find(tfd);
            //       if (it != eventCallbackMap_.end())
            //       {
            //           it->second();
            //       }
        }
        if (static_cast<size_t>(numEvents) == events_.size())
        {
            events_.resize(events_.size()*2);
        }
    }
    else if (numEvents == 0)
    {
    }
    else
    {
// error happens, log uncommon ones
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
        }
    }
    return numEvents;
}

int ChannelPoll::addChannel(const EventChannelPtr& ec)
{
    channelList_[ec->fd()] = ec;
    struct epoll_event ev;

    ev.events = ec->events();
    ev.data.ptr = static_cast<void*>(ec.get());
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, ec->fd(), &ev) < 0)
    {
        LOG_ERROR("add Channel error|fd=%d|errno=%d|errmsg=%s", ec->fd(), errno, strerror(errno));
        removeChannel(ec->fd());
        return -1;
    }
    else
    {
    }
    return 0;
}
int ChannelPoll::removeChannel(int fd)
{
    if(epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL) < 0)
    {
        //LOG_ERROR("remove Channel error|errno=%d|errmsg=%s", errno, strerror(errno));
        //return -1;
    }
    else
    {
    }
    channelList_.erase(fd);
    return 0;
}
int ChannelPoll::modifyChannel(const EventChannelPtr& ec)
{
    ChannelList::iterator it = channelList_.find(ec->fd());
    if (it == channelList_.end()) return -2;
    struct epoll_event ev;
    ev.events = ec->events();
    ev.data.ptr = static_cast<void*>(ec.get());
    if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, ec->fd(), &ev) < 0)
    {
        //LOG_ERROR("modify Channel error|errno=%d|errmsg=%s", errno, strerror(errno));
        removeChannel(ec->fd());
        return -1;
    }
    else
    {
    }
    return 0;
}
