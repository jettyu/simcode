#include <simcode/net/TcpServer.h>
#include <simcode/base/logger.h>
using namespace simcode;
using namespace net;

TcpServer::TcpServer(EventLoop* loop, const SockAddr& addr, const std::string&name):
    loop_(loop),
    acceptor_(addr, true),
    conntectionList_(new TcpConnMap),
    threadNum_(0)
{
}

void TcpServer::start()
{
    loopThreadPool_.setThreadNum(threadNum_);
    loopThreadPool_.start();
    acceptor_.setCallback(simex::bind(&TcpServer::onConnection, this, _1,_2));
    acceptor_.Listen();
    loop_->runInLoop(acceptor_.sockfd(), simex::bind(&Acceptor::Accept, &acceptor_));
}

void TcpServer::onClose(const TcpConnectionPtr& conn)
{
    if (closeCallback_) closeCallback_(conn);
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->removeInLoop(conn->connfd());
    LOG_DEBUG("client close|ip=%s|port=%u", conn->peerAddr().ip().c_str(), conn->peerAddr().port());
    conntectionList_->erase(conn->connfd());
    //{
    //ScopeLock lock(mutex_);
    //conntectionList_.erase(conn->connfd());
    //}
}

void TcpServer::onConnection(int connfd, const SockAddr& peerAddr)
{
    EventLoop* ioLoop = loopThreadPool_.getNextLoop();
    if (!ioLoop) ioLoop = loop_;
    TcpConnectionPtr conn(new TcpConnection(ioLoop, connfd, peerAddr));
    conn->setCloseCallback(simex::bind(&TcpServer::onClose, this, conn));
    conn->setMessageCallback(messageCallback_);
    LOG_DEBUG("new client|ip=%s|port=%u", peerAddr.ip().c_str(), peerAddr.port());
    if (connectionCallback_) connectionCallback_(conn);
    conntectionList_->add(conn->connfd(), conn);
    //ScopeLock lock(mutex_);
    //conntectionList_[conn->connfd()] = conn;
    conn->run();
}

void TcpConnMap::add(uint64_t id, const TcpConnectionPtr& conn)
{
    {
    WriteLock lock(mutex_);
    connMap_[id] = conn;
    }
}

TcpConnectionPtr TcpConnMap::get(uint64_t id)
{
    TcpConnectionPtr ptr;
    {
    ReadLock lock(mutex_);
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
        WriteLock lock(mutex_);
        connMap_.erase(id);
    }
}

