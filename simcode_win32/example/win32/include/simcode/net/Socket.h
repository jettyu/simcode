
#ifndef SN_SOCKET_H
#define SN_SOCKET_H

#include <simcode/base/noncopyable.h>
#include <simcode/base/typedef.h>
#include <simcode/net/SockAddr.h>
#ifdef WIN32

#include <winsock2.h>

#endif

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
        Close();
    }

    int sockfd() const
    {
        return sockfd_;
    }

    int Connect(const SockAddr& addr);
    int ConnectRetry(const SockAddr& addr, int retryNum=1);
    int Bind(const SockAddr& addr);
    int Listen(int backlog = 1024);
	int Accept(SockAddr* addr);
    void Close()
    {
#ifdef WIN32
        ::closesocket(sockfd_);
#else
		::close(sockfd_);
#endif
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

    int setNonBlockAndCloseOnExec();

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
