#include <simcode/net/Socket.h>
#include <simcode/net/SockAddr.h>

using namespace simcode;
using namespace simcode::net;

bool Socket::getTcpInfo(struct tcp_info* tcpi) const
{
    socklen_t len = sizeof(*tcpi);
    bzero(tcpi, len);
    return getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
}

bool Socket::getTcpInfoString(char* buf, int len) const
{
    struct tcp_info tcpi;
    bool ok = getTcpInfo(&tcpi);
    if (ok)
    {
        snprintf(buf, len, "unrecovered=%u "
                 "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
                 "lost=%u retrans=%u rtt=%u rttvar=%u "
                 "sshthresh=%u cwnd=%u total_retrans=%u",
                 tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
                 tcpi.tcpi_rto,          // Retransmit timeout in usec
                 tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
                 tcpi.tcpi_snd_mss,
                 tcpi.tcpi_rcv_mss,
                 tcpi.tcpi_lost,         // Lost packets
                 tcpi.tcpi_retrans,      // Retransmitted packets out
                 tcpi.tcpi_rtt,          // Smoothed round trip time in usec
                 tcpi.tcpi_rttvar,       // Medium deviation
                 tcpi.tcpi_snd_ssthresh,
                 tcpi.tcpi_snd_cwnd,
                 tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
    }
    return ok;
}

int Socket::Bind(const SockAddr& addr)
{
    return ::bind(sockfd_, addr.addr(), static_cast<socklen_t>(sizeof(*addr.addr())));
}

int Socket::Listen(int backlog)
{
    return ::listen(sockfd_, backlog);
}


int Socket::Accept(SockAddr* peeraddr)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));

    socklen_t addrlen = static_cast<socklen_t>(sizeof addr);
    int connfd = ::accept(sockfd_, (struct sockaddr*)(&addr), &addrlen);
    setNonBlockAndCloseOnExec(connfd);
    if (connfd >= 0)
    {
        peeraddr->set_addr_in(addr);
    }
    return connfd;
}

int Socket::Connect(const SockAddr& addr)
{
    setNonBlockAndCloseOnExec(sockfd_);
    return ::connect(sockfd_, addr.addr(), static_cast<socklen_t>(sizeof(*addr.addr())));
}

int Socket::ConnectRetry(const SockAddr& addr, int retryNum)
{
    setNonBlockAndCloseOnExec(sockfd_);
    int ret = 0;
    for (int nsec=1; nsec<=retryNum; nsec++)
    {
        if((ret=::connect(sockfd_, addr.addr(), static_cast<socklen_t>(sizeof(*addr.addr())))) == 0) break;
        sleep(nsec);
    }
    return ret;
}

int Socket::ShutdownWrite()
{
    return ::shutdown(sockfd_, SHUT_WR);
}

int Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    return ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                        &optval, static_cast<socklen_t>(sizeof(optval)));
    // FIXME CHECK
}

int Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    return ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                        &optval, static_cast<socklen_t>(sizeof(optval)));
    // FIXME CHECK
}

int Socket::setReusePort(bool on)
{
#ifdef SO_REUSEPORT
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                           &optval, static_cast<socklen_t>(sizeof optval));
    return ret;
#else
    return 0;
#endif
}

int Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    return ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
                        &optval, static_cast<socklen_t>(sizeof optval));
}

int Socket::setNonBlockAndCloseOnExec()
{
    Socket::setNonBlockAndCloseOnExec(sockfd_);
}

int Socket::setNonBlockAndCloseOnExec(int _sockfd)
{
    //set-nonblock
    int flags = ::fcntl(_sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(_sockfd, F_SETFL, flags);
    // close-on-exec
    flags = ::fcntl(_sockfd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(_sockfd, F_SETFD, flags);
    return ret;
}

int Socket::getError() const
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);

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
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
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
  socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
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
