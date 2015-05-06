#include <simcode/net/EPollPoller.h>
#include <simcode/net/CommonHead.h>
using namespace simcode;
using namespace simcode::net;
EPollPoller::EPollPoller():
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
    events_(kInitEventListSize)
{
    //ctor
}

EPollPoller::~EPollPoller()
{
    //dtor
    ::close(epollfd_);
}

int EPollPoller::poll(int timeoutMs)
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
            (*reinterpret_cast<const EventCallback*>(events_[i].data.ptr))(events_[i].events);
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

int EPollPoller::addEvent(int fd, const EventCallback& b, int events)
{
    struct epoll_event ev;

    ev.events = events;
    eventCallbackMap_[fd] = b;
    ev.data.ptr = (void*)&eventCallbackMap_[fd];
    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) < 0)
    {
        removeEvent(fd);
        return -1;
    }
    else
    {
    }
    return 0;
}
int EPollPoller::removeEvent(int fd)
{
    eventCallbackMap_.erase(fd);
    return epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL);
}
int EPollPoller::modifyEvent(int fd, int events)
{
    EventCallbackMap::iterator it = eventCallbackMap_.find(fd);
    if (it == eventCallbackMap_.end()) return -2;
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = (void*)&eventCallbackMap_[fd];
    if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev) < 0)
    {
        removeEvent(fd);
        return -1;
    }
    else
    {
    }
    return 0;
}
