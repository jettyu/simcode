#include <simcode/net/UdpServer.h>
#include <simcode/net/NetLogger.h>
using namespace simcode;
using namespace net;
using namespace thread;

UdpServer::UdpServer(EventLoop* loop, const SockAddr& addr, const std::string& name):
    loop_(loop),
    socket_(PF_INET, SOCK_DGRAM, 0),
    threadNum_(0)
{
    assert(socket_.sockfd() != -1);
    socket_.setReuseAddr();
    socket_.setReusePort();
    socket_.setNonBlockAndCloseOnExec();
    assert(socket_.Bind(addr) != -1);
}

void UdpServer::start()
{
    queue_.setThreadNum(threadNum_+1);
    queue_.start();
    loop_->runInLoop(socket_.sockfd(), SimBind(&UdpServer::eventHandle, this, _1));
}

void UdpServer::setThreadNum(int n)
{
    threadNum_ = n;
}

void UdpServer::eventHandle(int events)
{
     queue_.push_back(threadNum_+1, SimBind(&UdpServer::hanleRead, this));
}

void UdpServer::hanleRead()
{
    int n = 0;
    do
    {
        UdpConnector c(socket_.sockfd());
        std::string buf;
        
        if ((n=c.Recv(&buf)) < 0)
        {
            //log error
            NETLOG_ERROR("read error|errno=%d|errmsg=%s", errno, strerror(errno));
            return;
        }
        uint64_t id = c.peerAddr().id();
        UdpConnectionPtr conn = connManager_.get(id);
        if (!conn)
        {
            NETLOG_DEBUG("recv new client|ip=%s|port=%u|id=%lu", c.peerAddr().ip().c_str(), c.peerAddr().port(), id);
            conn.reset(new UdpConnection(c));
            conn->setCloseCallback(SimBind(&UdpServer::removeConnection, this, _1));
            connManager_.add(id, conn);
        }
        //onMessage(conn, buf);
        if (threadNum_)
            queue_.push_back(conn->id()%threadNum_, SimBind(&UdpServer::onMessage, this, conn, &buf));
        else
            onMessage(conn, &buf);
    } while (n > 0);
}

void UdpServer::onMessage(const UdpConnectionPtr& c, std::string* msg)
{
    messageCallback_(c, msg);
}

void UdpServer::removeConnection(int64_t connId)
{
    NETLOG_DEBUG("client remove|id=%ld", connId);
    connManager_.erase(connId);
}
