#include <simcode/net/TcpServer.h>
#include <simcode/base/logger.h>
using namespace simcode;
using namespace net;

TcpServer::TcpServer(EventLoop* loop, const SockAddr& addr, const std::string&name, bool reuseport):
    loop_(loop),
    acceptor_(addr, reuseport),
    conntectionList_(new TcpConnMap),
    threadNum_(0)
{
    acceptChannel_.reset(new EventChannel(loop, acceptor_.sockfd(), simex::bind(&TcpServer::acceptHandler, this, _1)));
    acceptChannel_->enableReading();
}

void TcpServer::start()
{
    loopThreadPool_.setThreadNum(threadNum_);
    loopThreadPool_.start();
    acceptor_.setCallback(simex::bind(&TcpServer::onConnection, this, _1,_2));
    int ret = acceptor_.Listen();
    LOG_DEBUG("ret=%d", ret);
    assert(ret == 0);

    loop_->runInLoop(acceptChannel_);
}

void TcpServer::onClose(const TcpConnectionPtr& conn)
{
    if (closeCallback_) closeCallback_(conn);
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->removeInLoop(conn->connfd());
    LOG_DEBUG("client close|ip=%s|port=%u", conn->peerAddr().ip().c_str(), conn->peerAddr().port());
    conntectionList_->erase(conn->connfd());
}

void TcpServer::onConnection(int connfd, const SockAddr& peerAddr)
{
    EventLoop* ioLoop = loopThreadPool_.getNextLoop();
    if (!ioLoop) ioLoop = loop_;
    TcpConnectionPtr conn(new TcpConnection(ioLoop, connfd, peerAddr));
    conn->setCloseCallback(simex::bind(&TcpServer::onClose, this, _1));
    conn->setMessageCallback(messageCallback_);
    LOG_DEBUG("new client|ip=%s|port=%u", peerAddr.ip().c_str(), peerAddr.port());
    if (connectionCallback_) connectionCallback_(conn);
    conntectionList_->add(conn->connfd(), conn);
    conn->run();
}

void TcpServer::acceptHandler(EventChannel*)
{
    acceptor_.Accept();
}

void TcpConnMap::add(uint64_t id, const TcpConnectionPtr& conn)
{
    {
    ScopeLock lock(mutex_);
    connMap_[id] = conn;
    }
}

TcpConnectionPtr TcpConnMap::get(uint64_t id)
{
    TcpConnectionPtr ptr;
    {
    ScopeLock lock(mutex_);
    std::map<uint64_t, TcpConnectionPtr>::iterator it;
    if ((it=connMap_.find(id)) != connMap_.end())
    {
        ptr = it->second;
    }
    }
    return ptr;
}

void TcpConnMap::erase(uint64_t id)
{
    {
        ScopeLock lock(mutex_);
        connMap_.erase(id);
    }
}

