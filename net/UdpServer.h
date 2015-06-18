#ifndef SIMCODE_NET_UDPSERVER_H
#define SIMCODE_NET_UDPSERVER_H
#include <simcode/thread/thread_safe_queue.h>
#include <simcode/net/EventLoop.h>
#include <simcode/net/SockAddr.h>
#include <simcode/net/Socket.h>
#include <simcode/net/UdpConnection.h>
#include <simcode/net/VecMap.h>
#include <simcode/net/ConnManager.h>
namespace simcode
{
namespace net
{
typedef BaseConnManager<UdpConnectionPtr> BaseUdpConnManager;
class UdpConnMap : public BaseUdpConnManager
{
public:
    void add(uint64_t id, const UdpConnectionPtr& conn);
    UdpConnectionPtr get(uint64_t id);
    void erase(uint64_t);
private:
    SharedMutex mutex_;
    std::map<uint64_t, UdpConnectionPtr> connMap_;
};

class UdpServer : noncopyable
{
public:
    typedef simex::function<void (const UdpConnectionPtr&, const std::string& msg)> MessageCallback;
    UdpServer(EventLoop* loop, const SockAddr& addr, const std::string& name);
    void start();
    void setThreadNum(int n);
    void setMessageCallback(const MessageCallback& c)
    {
        messageCallback_ = c;
    }
private:
    void onMessage(const UdpConnectionPtr& c, const std::string& msg);
    void eventHandle(int events);
    void hanleRead();
    void removeConnection(int64_t connId);
private:
    simcode::net::EventLoop* loop_;
    simcode::thread::ThreadSafeQueue queue_;
    Socket socket_;
    MessageCallback messageCallback_;
    SharedPtr<BaseUdpConnManager> conntectionList_;
    //std::map<uint64_t, UdpConnectionPtr> connManager_;
    //Mutex mutex_;
    int threadNum_;
};
}
}
#endif // UDP_SERVER_H
