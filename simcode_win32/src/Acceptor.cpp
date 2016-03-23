#include <simcode/net/Acceptor.h>
#include <simcode/base/logger.h>
using namespace simcode;
using namespace simcode::net;
Acceptor::Acceptor(const SockAddr& listenAddr, bool reuseport):
    socket_(AF_INET, SOCK_STREAM, IPPROTO_TCP),
    isListening_(false)
{
    socket_.setReuseAddr(true);
    socket_.setReusePort(reuseport);
	socket_.setNonBlockAndCloseOnExec();
    int ret = socket_.Bind(listenAddr);
    LOG_DEBUG("ret=%d", ret);
    //socket_.setNonBlockAndCloseOnExec();
}

Acceptor::~Acceptor()
{
}

int Acceptor::Listen(int backlog)
{
    isListening_ = true;
    return socket_.Listen(backlog);
}

int Acceptor::Accept()
{
    SockAddr peerAddr;
//FIXME loop until no more
    int connfd = socket_.Accept(&peerAddr);
    if (connfd >= 0)
    {
        if (newConnectionCallback_) newConnectionCallback_(connfd, peerAddr);
		else ::closesocket(connfd);
    }
    else
    {
        if (errno == EMFILE)
        {
			LOG_ERROR("too many clients");
        }
    }
    return 0;
}
