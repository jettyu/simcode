#include <simcode/net/Selector.h>
using namespace simcode;
using namespace net;
int Selector::poll(int sec, int usec)
{
        FD_ZERO(&readfds_);
        FD_ZERO(&writefds_);
        FD_ZERO(&exceptfds_);
        readfds_ = allfds_;
        writefds_ = allfds_;
        int ret;
        timeout_.tv_sec = sec;
        timeout_.tv_usec = usec;
        ret = select(0, &readfds_, &writefds_, &exceptfds_, &timeout_);
        if (SOCKET_ERROR == ret) return ret;
		int validn = 0;
        for (u_int i=0; i<allfds_.fd_count; i++)
        {
			int validflag = 0;
			simex::shared_ptr<Channel> ch = channels_[i].lock();
			if (!ch) continue;
            if (FD_ISSET(allfds_.fd_array[i], &readfds_))
            {
				validflag = 1;
                //recv
                
                if (ch) ch->handleRead();
            }
            if (FD_ISSET(allfds_.fd_array[i], &writefds_))
            {
				validflag = 1;
                //send
                simex::shared_ptr<Channel> ch = channels_[i].lock();
                if (ch) ch->handleWrite();
            }
            if (FD_ISSET(allfds_.fd_array[i], &exceptfds_))
            {
				validflag = 1;
                //handle error
                simex::shared_ptr<Channel> ch = channels_[i].lock();
                if (ch) ch->handleError();
            }
			validn += validflag;
        }
        return ret;
    }
