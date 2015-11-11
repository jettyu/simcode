#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <simcode/net/UdpConnection.h>
#include <simcode/net/EventLoop.h>
#include <simcode/base/noncopyable.h>
#include <simcode/net/Socket.h>
#include <simcode/net/EventChannel.h>
namespace simcode
{
namespace net
{
class UdpClient;
typedef simex::shared_ptr<UdpClient> UdpClientPtr;
class UdpClient : noncopyable
{
public:
    typedef simex::function<void (const UdpConnectionPtr&, const char* data, size_t size)> MessageCallback;
    UdpClient(EventLoop*, const SockAddr&, const std::string& name);
    int send(const char* buf, size_t len);
    int sendString(const std::string& data);
    void setMessageCallback(const MessageCallback& c)
    {
        messageCallback_ = c;
    }
    UdpConnectionPtr getConn() const
    {
        return conn_;
    }
private:
    void eventHandle(EventChannel*);
private:
    Socket socket_;
    UdpConnectionPtr conn_;
    MessageCallback messageCallback_;
    EventChannelPtr channel_;

};

}
}

#endif
