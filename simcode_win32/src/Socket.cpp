#include <simcode/net/Socket.h>
#include <stdio.h>
using namespace simcode;
using namespace net;
int Socket::Connect(const SockAddr& addr)
{
    setNonBlockAndCloseOnExec(sockfd_);
    return connect(sockfd_, addr.addr(), (sizeof(*addr.addr())));
}

int Socket::ConnectRetry(const SockAddr& addr, int retryNum)
{
    setNonBlockAndCloseOnExec(sockfd_);
    int ret = 0;
    for (int nsec=1; nsec<=retryNum; nsec++)
    {
        if((ret=::connect(sockfd_, addr.addr(), (sizeof(*addr.addr())))) == 0) break;
        Sleep(nsec);
    }
    return ret;
}

int Socket::Bind(const SockAddr& addr)
{
    return ::bind(sockfd_, addr.addr(), sizeof(*addr.addr()));
}

int Socket::Listen(int backlog)
{
    return ::listen(sockfd_, backlog);
}

int Socket::Accept(SockAddr* peeraddr)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));

    int addrlen = sizeof addr;
    int connfd = ::accept(sockfd_, (struct sockaddr*)(&addr), &addrlen);
    setNonBlockAndCloseOnExec(connfd);
    if (connfd >= 0)
    {
        peeraddr->set_addr_in(addr);
    }
    return connfd;
}

int Socket::ShutdownWrite()
{
    return ::shutdown(sockfd_, SD_SEND);
}

int Socket::setTcpNoDelay(bool on)
{
    char optval = on ? 1 : 0;
    return ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                        &optval, (sizeof(optval)));
    // FIXME CHECK
}

int Socket::setReuseAddr(bool on)
{
    char optval = on ? 1 : 0;
    return ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                        &optval, (sizeof(optval)));
    // FIXME CHECK
}

int Socket::setReusePort(bool on)
{
#ifdef SO_REUSEPORT
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                           &optval, (sizeof optval));
    return ret;
#else
    return 0;
#endif
}

int Socket::setKeepAlive(bool on)
{
    char optval = on ? 1 : 0;
    return ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
                        &optval, (sizeof optval));
}

int Socket::setNonBlockAndCloseOnExec()
{
    return Socket::setNonBlockAndCloseOnExec(sockfd_);
}

int Socket::setNonBlockAndCloseOnExec(int _sockfd)
{
    char optval = 1;
    return ::setsockopt(_sockfd, IPPROTO_TCP, TCP_NODELAY,
                        &optval, (sizeof(optval)));
}

int Socket::getError() const
{
    char optval;
    int optlen = (sizeof optval);

    if (::getsockopt(sockfd_, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
    {
        return errno;
    }
    else
    {
        return optval;
    }
}

struct sockaddr_in Socket::getLocalAddr() const
{
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof localaddr);
    int addrlen = (sizeof localaddr);
    if (::getsockname(sockfd_, (struct sockaddr*)(&localaddr), &addrlen) < 0)
    {
        //LOG_SYSERR << "sockets::getLocalAddr";
    }
    return localaddr;
}

struct sockaddr_in Socket::getPeerAddr() const
{
    struct sockaddr_in peeraddr;
    bzero(&peeraddr, sizeof peeraddr);
    int addrlen = (sizeof peeraddr);
    if (::getpeername(sockfd_, (struct sockaddr*)(&peeraddr), &addrlen) < 0)
    {
        //LOG_SYSERR << "sockets::getPeerAddr";
    }
    return peeraddr;
}

bool Socket::isSelfConnect() const
{
    struct sockaddr_in localaddr = getLocalAddr();
    struct sockaddr_in peeraddr = getPeerAddr();
    return localaddr.sin_port == peeraddr.sin_port
           && localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;
}
