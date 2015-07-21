#include <simcode/net/UdpServer.h>
#include <simcode/net/EventChannel.h>
#include <simcode/base/logger.h>
using namespace simcode;
using namespace net;
using namespace thread;

UdpServer::UdpServer(EventLoop* loop, const SockAddr& addr, const std::string& name):
    loop_(loop),
    socket_(PF_INET, SOCK_DGRAM, 0),
    conntectionList_(new UdpConnMap),
    threadNum_(0)
{
    assert(socket_.sockfd() != -1);
    socket_.setReuseAddr();
    socket_.setReusePort();
    socket_.setNonBlockAndCloseOnExec();
    assert(socket_.Bind(addr) != -1);
    channel_.reset(new EventChannel(loop, socket_.sockfd(), simex::bind(&UdpServer::eventHandle, this, _1)));
    channel_->enableReading();
}

void UdpServer::start()
{
    queue_.setThreadNum(threadNum_+1);
    queue_.start();
    loop_->runInLoop(channel_);
}

void UdpServer::setThreadNum(int n)
{
    threadNum_ = n;
}

void UdpServer::eventHandle(EventChannel*)
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
            //LOG_DEBUG("read error|errno=%d|errmsg=%s", errno, strerror(errno));
            return;
        }
        uint64_t id = c.peerAddr().id();
        //UdpConnectionPtr conn = conntectionList_.get(id);
        UdpConnectionPtr conn = conntectionList_->get(id);
        if (!conn)
        {
            LOG_DEBUG("recv new client|ip=%s|port=%u|id=%lu", c.peerAddr().ip().c_str(), c.peerAddr().port(), id);
            conn.reset(new UdpConnection(c));
            conn->setCloseCallback(SimBind(&UdpServer::removeConnection, this, _1));
            conntectionList_->add(id, conn);
        }
        //onMessage(conn, buf);
        if (threadNum_)
            queue_.push_back(conn->id()%threadNum_, SimBind(&UdpServer::onMessage, this, conn, buf));
        else
            onMessage(conn, buf);
    }while (n > 0);
}

void UdpServer::onMessage(const UdpConnectionPtr& c, const std::string& msg)
{
    messageCallback_(c, msg);
}

void UdpServer::removeConnection(int64_t connId)
{
    LOG_DEBUG("client remove|id=%ld", connId);
    conntectionList_->erase(connId);
    //{
    //ScopeLock lock(mutex_);
    //conntectionList_.erase(connId);
    //}
}

void UdpConnMap::add(uint64_t id, const UdpConnectionPtr& conn)
{
    {
    ScopeLock lock(mutex_);
    connMap_[id] = conn;
    }
}

UdpConnectionPtr UdpConnMap::get(uint64_t id)
{
    UdpConnectionPtr ptr;
    {
    ScopeLock lock(mutex_);
    std::map<uint64_t, UdpConnectionPtr>::iterator it;
    if ((it=connMap_.find(id)) != connMap_.end())
    {
        ptr = it->second;
    }
    }
    return ptr;
}

void UdpConnMap::erase(uint64_t id)
{
    {
        ScopeLock lock(mutex_);
        connMap_.erase(id);
    }
}

