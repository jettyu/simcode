#ifndef SELECTOR_H
#define SELECTOR_H

#ifdef WIN32
#include <winsock2.h>
#endif
#include <simcode/net/Channel.h>
#include <simcode/base/noncopyable.h>
#include <vector>

#define MAX_SELECT_NUM 1024

namespace simcode
{
namespace net
{

class Selector : noncopyable
{
public:
    Selector() : channels_(MAX_SELECT_NUM)
    {
        FD_ZERO(&allfds_);
    }
    ~Selector()
    {
    }
    void addChannel(const simex::shared_ptr<Channel>& c)
    {
        FD_SET(c->fd(), &allfds_);
        channels_[allfds_.fd_count-1] = c;
    }
    void removeChannel(int fd)
    {
        FD_CLR(fd, &allfds_);
    }

    int poll(int sec, int usec);

private:
    fd_set readfds_;
    fd_set writefds_;
    fd_set exceptfds_;
    fd_set allfds_;
    timeval timeout_;
    std::vector<simex::weak_ptr<Channel> > channels_;
};

}
}
#endif // SELECTOR_H
