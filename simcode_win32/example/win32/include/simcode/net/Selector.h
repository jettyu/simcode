#ifndef SELECTOR_H
#define SELECTOR_H

#ifdef WIN32
#include <winsock2.h>
#endif
#include <simcode/base/typedef.h>
#include <simcode/base/noncopyable.h>
#include <map>

#define MAX_SELECT_NUM 1024

namespace simcode
{
namespace net
{
class EventChannel;
typedef simex::shared_ptr<EventChannel> EventChannelPtr;
class Selector : noncopyable
{
public:
    Selector()
    {
        FD_ZERO(&allfds_);
		FD_ZERO(&readfds_);
		FD_ZERO(&writefds_);
    }
    ~Selector()
    {
    }
    void addChannel(const simex::shared_ptr<EventChannel>& c);
	void removeChannel(int fd);
	void modifyChannel(const simex::shared_ptr<EventChannel>& c);
    int poll(int sec, int usec);

private:
    fd_set readfds_;
    fd_set writefds_;
    fd_set allfds_;
    timeval timeout_;
    std::map<int, simex::weak_ptr<EventChannel> > channels_;
};

}
}
#endif // SELECTOR_H
