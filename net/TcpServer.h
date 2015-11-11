#ifndef SIMCODE_NET_TCPSERVER_H
#define SIMCODE_NET_TCPSERVER_H
#include <simcode/thread/thread_safe_queue.h>
#include <simcode/net/EventLoop.h>
#include <simcode/net/SockAddr.h>
#include <simcode/net/Socket.h>
#include <simcode/net/TcpConnection.h>
#include <simcode/net/EventLoopThreadPool.h>
#include <simcode/net/Acceptor.h>
#include <simcode/net/ConnManager.h>
#include <simcode/net/EventChannel.h>
namespace simcode
{
namespace net
{
typedef ConnManager<uint64_t, TcpConnectionPtr> TcpConnectionManager;
typedef simex::shared_ptr<TcpConnectionManager> TcpConnectionManagerPtr;
class TcpServer : noncopyable
{
public:
    typedef simex::function<void (const TcpConnectionPtr&, Buffer* msg)> MessageCallback;
    typedef simex::function<void (const TcpConnectionPtr&)> ConnectionCallback;
    typedef simex::function<void (const TcpConnectionPtr&)> CloseCallback;
    TcpServer(EventLoop* loop,
              const SockAddr& addr,
              const std::string& name,
              bool reuseport = false);
    void start();
    void setThreadNum(int n)
    {
        threadNum_ = n;
    }
    //default accept loop
    void setSendLoop(EventLoop* loop)
    {
        sendLoop_ = loop;
    }
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
    const TcpConnectionManagerPtr& getConnectionManager() const
    {
        return connectionManager_;
    }
private:
    void onConnection(int connfd, const SockAddr& peerAddr);
    void onClose(const TcpConnectionPtr&);
    void acceptHandler(EventChannel*);
    void send(const TcpConnectionPtr& conn, const std::string& data);
private:
    simcode::net::EventLoop* loop_;
    MessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;
    CloseCallback closeCallback_;
    Acceptor acceptor_;
    EventChannelPtr acceptChannel_;
    int threadNum_;
    EventLoopThreadPool loopThreadPool_;
    simex::atomic_uint connectionId_;
    TcpConnectionManagerPtr connectionManager_;
    EventLoop* sendLoop_;
};
}
}

#endif
