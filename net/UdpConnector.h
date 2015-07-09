#ifndef UDP_CONNECTOR_H
#define UDP_CONNECTOR_H

#include <simcode/base/typedef.h>
#include <simcode/net/SockAddr.h>
#include <simcode/base/copyable.h>
namespace simcode
{
namespace net
{
class UdpConnector : copyable
{
public:
    UdpConnector(int sockfd__);
    int errcode() const
    {
        return errcode_;
    }
    int Recv(std::string* buf);
    int SendString(const std::string&);
    int Send(const char* buf, size_t len);
    void Close()
    {
        ::close(sockfd_);
    }
    void setPeerAddr(const SockAddr& addr)
    {
        peerAddr_ = addr;
    }
    const SockAddr& peerAddr() const
    {
        return peerAddr_;
    }
    int sockfd() const
    {
        return sockfd_;
    }
private:
    int sockfd_;
    int errcode_;
    SockAddr peerAddr_;
    socklen_t addrLen_;
};
}
}
#endif // UDP_CONNECTOR_H
