#ifndef SIMCODE_NET_UDPSERVER_H
#define SIMCODE_NET_UDPSERVER_H
#include <simcode/thread/thread_safe_queue.h>
#include <simcode/net/EventLoop.h>
#include <simcode/net/SockAddr.h>
#include <simcode/net/Socket.h>
#include <simcode/net/UdpConnection.h>
#include <simcode/net/ConnManager.h>
namespace simcode
{
namespace net
{

typedef ConnManager<uint64_t, UdpConnectionPtr> UdpConnectionManager;
typedef simex::shared_ptr<UdpConnectionManager> UdpConnectionManagerPtr;
class UdpServer : noncopyable
{
public:
    typedef simex::function<void (const UdpConnectionPtr&, const char* data, size_t size)> MessageCallback;
    UdpServer(EventLoop* loop, const SockAddr& addr, const std::string& name);
    void start();
    void setThreadNum(int n);
    void setSendLoop(EventLoop* loop)
    {
        sendLoop_ = loop;
    }
    void setMessageCallback(const MessageCallback& c)
    {
        messageCallback_ = c;
    }
    const UdpConnectionManagerPtr& getConnectionManager() const
    {
        return connectionManager_;
    }
private:
    void onMessage(const simex::any& connStore);
    void asyncMessage(const UdpConnectionPtr& conn, const std::string& data);
    void eventHandle(EventChannel*);
    void hanleRead();
    void onClose(uint64_t connId);
    void send(const UdpConnectionPtr& conn, const std::string& data);
private:
    simcode::net::EventLoop* loop_;
    simcode::thread::ThreadSafeQueue queue_;
    Socket socket_;
    MessageCallback messageCallback_;
    EventChannelPtr channel_;
    int threadNum_;
    UdpConnectionManagerPtr connectionManager_;
    EventLoop* sendLoop_;
};
}
}
#endif // UDP_SERVER_H
