#ifndef SN_SOCKET_H
#define SN_SOCKET_H

#include <simcode/net/SockAddr.h>
#include <simcode/net/CommonHead.h>
#include <simcode/base/noncopyable.h>
namespace simcode
{
namespace net
{

class Socket : noncopyable
{
public:
    /** Default constructor */
    Socket(int _sockfd):sockfd_(_sockfd) {}
    Socket(int domain, int type, int protocol)
        : sockfd_(::socket(domain, type, protocol))
    {
    }
    // Socket(Socket&&) // move constructor in C++11
    ~Socket()
    {
        ::close(sockfd_);
    }

    int sockfd() const
    {
        return sockfd_;
    }

    // return true if success.
    bool getTcpInfo(struct tcp_info*) const;
    bool getTcpInfoString(char* buf, int len) const;

    /// abort if address in use
    int Bind(const SockAddr& addr);
    /// abort if address in use
    int Listen(int backlog = 1024);


    int Accept(SockAddr* addr);

    int Connect(const SockAddr& addr);
    int ConnectRetry(const SockAddr& addr, int retryNum=1);

    void Close()
    {
        ::close(sockfd_);
    }
    int ShutdownWrite();
//        void ShutdownRead();

    int setTcpNoDelay(bool on);

    ///
    /// Enable/disable SO_REUSEADDR
    ///
    int setReuseAddr(bool on = true);

    ///
    /// Enable/disable SO_REUSEPORT
    ///
    int setReusePort(bool on = true);

    ///
    /// Enable/disable SO_KEEPALIVE
    ///
    int setKeepAlive(bool on = true);

    int setNonBlock();
    int setNonBlockAndCloseOnExec();

    static int setNonBlock(int sockfd);
    static int setNonBlockAndCloseOnExec(int sockfd);
    int getError() const;
    struct sockaddr_in getLocalAddr() const;
    struct sockaddr_in getPeerAddr() const;
    bool isSelfConnect() const;
private:
    int sockfd_;
};

}
}

#endif // SN_SOCKET_H
