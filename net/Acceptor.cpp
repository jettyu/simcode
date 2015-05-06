#include <simcode/net/Acceptor.h>

using namespace simcode;
using namespace simcode::net;
Acceptor::Acceptor(const SockAddr& listenAddr, bool reuseport):
    socket_(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP),
    isListening_(false),
    idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    socket_.setReuseAddr(true);
    socket_.setReusePort(reuseport);
    socket_.setNonBlockAndCloseOnExec();
    socket_.Bind(listenAddr);
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
        else ::close(connfd);
    }
    else
    {
        if (errno == EMFILE)
        {
            ::close(idleFd_);
            idleFd_ = ::accept(socket_.sockfd(), NULL, NULL);
            ::close(idleFd_);
            idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
    return 0;
}
