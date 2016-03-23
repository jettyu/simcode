#include <simcode/net/Selector.h>
#include <simcode/net/EventChannel.h>
using namespace simcode;
using namespace net;
int Selector::poll(int sec, int usec)
{
	fd_set readfds, writefds, exceptfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
	readfds = readfds_;
	writefds = writefds_;
	exceptfds = allfds_;
    int ret;
    timeout_.tv_sec = sec;
    timeout_.tv_usec = usec;
    ret = select(0, &readfds, &writefds, &exceptfds, &timeout_);
    if (SOCKET_ERROR == ret)
	{
		return ret;
	}
	int validn = 0;
	for (u_int i=0; i<allfds_.fd_count && validn < ret; i++)
    {
		simex::shared_ptr<EventChannel> ch = channels_[i].lock();
		if (!ch) continue;
        if (FD_ISSET(allfds_.fd_array[i], &readfds))
        {
            //recv
			ch->setReventReading();
        }
        if (FD_ISSET(allfds_.fd_array[i], &writefds))
        {
            //send
			ch->setReventWriting();
        }
        if (FD_ISSET(allfds_.fd_array[i], &exceptfds))
        {
            //handle error
			ch->setReventError();
        }
		if (!ch->isNoneEvent()) 
		{
			validn++;
			ch->handleEvent(ch->revents());
		}
    }
    return ret;
}


void Selector::addChannel(const simex::shared_ptr<EventChannel>& c)
{
    FD_SET(c->fd(), &allfds_);
    channels_[allfds_.fd_count-1] = c;

	if (c->isEnableReading())
		FD_SET(c->fd(), &readfds_);
	if (c->isEnableWriting())
		FD_SET(c->fd(), &writefds_);
}
void Selector::removeChannel(int fd)
{
    FD_CLR(fd, &allfds_);
	FD_CLR(fd, &readfds_);
	FD_CLR(fd, &writefds_);
	channels_.erase(fd);
}
void Selector::modifyChannel(const simex::shared_ptr<EventChannel>& c)
{
	if (!FD_ISSET(c->fd(), &allfds_)) return;
	if (c->isEnableReading())
		FD_SET(c->fd(), &readfds_);
	else if (FD_ISSET(c->fd(), &readfds_))
		FD_CLR(c->fd(), &readfds_);
	if (c->isEnableWriting()) 
		FD_SET(c->fd(), &writefds_);
	else if (FD_ISSET(c->fd(), &writefds_))
		FD_CLR(c->fd(), &writefds_);
}
