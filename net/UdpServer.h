#ifndef SIMCODE_NET_UDPSERVER_H
#define SIMCODE_NET_UDPSERVER_H
#include <simcode/thread/thread_safe_queue.h>
#include <simcode/net/EventLoop.h>
#include <simcode/net/SockAddr.h>
#include <simcode/net/Socket.h>
#include <simcode/net/UdpConnection.h>
#include <simcode/net/VecMap.h>
namespace simcode
{
namespace net
{
class UdpServer : noncopyable
{
public:
    typedef simex::function<void (const UdpConnectionPtr&, std::string* msg)> MessageCallback;
    UdpServer(EventLoop* loop, const SockAddr& addr, const std::string& name);
    void start();
    void setThreadNum(int n);
    void setMessageCallback(const MessageCallback& c)
    {
        messageCallback_ = c;
    }
private:
    void onMessage(const UdpConnectionPtr& c, std::string* msg);
    void eventHandle(int events);
    void hanleRead();
    void removeConnection(int64_t connId);
private:
    simcode::net::EventLoop* loop_;
    simcode::thread::ThreadSafeQueue queue_;
    Socket socket_;
    MessageCallback messageCallback_;
    VecMap16<UdpConnectionPtr> connManager_;
    int threadNum_;
};
}
}
#endif // UDP_SERVER_H
