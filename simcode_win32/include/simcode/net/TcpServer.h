#ifndef SIMCODE_NET_TCPSERVER_H
#define SIMCODE_NET_TCPSERVER_H
#include <simcode/thread/thread_safe_queue.h>
#include <simcode/net/EventLoop.h>
#include <simcode/net/SockAddr.h>
#include <simcode/net/Socket.h>
#include <simcode/net/TcpConnection.h>
#include <simcode/net/EventLoopThreadPool.h>
#include <simcode/net/Acceptor.h>
#include <simcode/net/EventChannel.h>
#include <simcode/net/EventChannel.h>
namespace simcode
{
namespace net
{

class TcpServer : noncopyable
{
public:
	typedef TcpConnection::Ptr TcpConnectionPtr;
    typedef simex::function<void (const TcpConnection::Ptr&, Buffer* msg)> MessageCallback;
    typedef simex::function<void (const TcpConnection::Ptr&)> ConnectionCallback;
    typedef simex::function<void (const TcpConnection::Ptr&)> CloseCallback;
    TcpServer(EventLoop* loop, const SockAddr& addr, const std::string& name, bool reuseport = false);
    void start();
    void setThreadNum(int n)
    {
        threadNum_ = n;
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
private:
    void onConnection(int connfd, const SockAddr& peerAddr);
    void onClose(const TcpConnectionPtr&);
    void acceptHandler(EventChannel*);

	void connectListErase(uint64_t id);
	void connectListAdd(const TcpConnectionPtr&);
private:
    simcode::net::EventLoop* loop_;
    simcode::thread::ThreadSafeQueue queue_;
    MessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;
    CloseCallback closeCallback_;
    Acceptor acceptor_;
    EventChannelPtr acceptChannel_;
	typedef std::map<uint64_t, TcpConnectionPtr> ConnectionList;
	ConnectionList connectionList_;
	simex::atomic_uint64_t connid_;
    int threadNum_;
    EventLoopThreadPool loopThreadPool_;
};
}
}

#endif
