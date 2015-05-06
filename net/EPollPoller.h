#ifndef SN_EPOLLPOLLER_H
#define SN_EPOLLPOLLER_H
#include <vector>
#include <map>
#include <simcode/base/typedef.h>
#include <simcode/net/CommonHead.h>
typedef struct epoll_event epoll_event_t;
namespace simcode
{
namespace net
{
class EPollPoller
{
public:
    typedef SimFunction<void(int events)> EventCallback;
    /** Default constructor */
    EPollPoller();
    /** Default destructor */
    virtual ~EPollPoller();
    int epollfd() const
    {
        return epollfd_;
    }
    int poll (int timeoutMs);
    int addEvent(int fd, const EventCallback& b, int events=EPOLLIN|EPOLLPRI);
    int removeEvent(int fd);
    int modifyEvent(int fd, int events);
protected:
private:
    static const int kInitEventListSize = 16;
    typedef std::vector<epoll_event_t> EventList;
    typedef std::map<int, EventCallback> EventCallbackMap;
    int epollfd_;
    int wakeupfd_;
    EventCallbackMap eventCallbackMap_;
    EventList events_;
};
}
}
#endif // SN_EPOLLPOLLER_H
