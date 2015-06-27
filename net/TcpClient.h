#ifndef SIMCODE_NET_TCP_CLIENT_H
#define SIMCODE_NET_TCP_CLIENT_H
#include <simcode/net/EventLoop.h>
#include <simcode/net/Connector.h>
#include <simcode/net/TcpConnection.h>
namespace simcode
{
namespace net
{
class TcpClient
{
public:
    typedef simex::function<void (const TcpConnectionPtr&, Buffer* msg)> MessageCallback;
    typedef simex::function<void (const TcpConnectionPtr&)> ConnectionCallback;
    typedef simex::function<void (const TcpConnectionPtr&)> CloseCallback;
    TcpClient(EventLoop* loop, const SockAddr& addr, const std::string& name);
    ~TcpClient();
    void active();
    void send(const char* data, size_t len);
    void send(const std::string& data);
    void setMessageCallback(const MessageCallback& c)
    {
        messageCallback_ = c;
    }
    void setConnectionCallback(const ConnectionCallback& b)
    {
        connectionCallback_ = b;
    }
    void setCloseCallback(const CloseCallback& b)
    {
        closeCallback_ = b;
    }

    EventLoop* getLoop() const
    {
        return loop_;
    }
    void setRetry(bool retry)
    {
        retry_ = retry;
    }
private:
    void onConnect();
    void onClose(const TcpConnectionPtr&);
    EventLoop* loop_;
    Connector connector_;
    TcpConnectionPtr conn_;
    MessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;
    CloseCallback closeCallback_;
    Mutex mutex_;
    bool retry_;
};
}
}
#endif
