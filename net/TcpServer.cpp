#include <simcode/net/TcpServer.h>
#include <simcode/base/logger.h>
using namespace simcode;
using namespace net;

TcpServer::TcpServer(EventLoop* loop,
                     const SockAddr& addr,
                     const std::string&name,
                     bool reuseport,
                     const TcpConnectionManagerPtr& cm):
    loop_(loop),
    acceptor_(addr, reuseport),
    connectionManager_(cm),
    threadNum_(0),
    connectionId_(0)
{
    acceptChannel_.reset(new EventChannel(loop, acceptor_.sockfd(), simex::bind(&TcpServer::acceptHandler, this, _1)));
    acceptChannel_->enableReading();
    if (!connectionManager_) connectionManager_.reset(new TcpConnectionManager(loop));
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
    connectionManager_->Remove(conn->id());
}

void TcpServer::onConnection(int connfd, const SockAddr& peerAddr)
{
    EventLoop* ioLoop = loopThreadPool_.getNextLoop();
    if (!ioLoop) ioLoop = loop_;
    TcpConnectionPtr conn(new TcpConnection(ioLoop, connfd, peerAddr, ++connectionId_));
    conn->setCloseCallback(simex::bind(&TcpServer::onClose, this, _1));
    conn->setMessageCallback(messageCallback_);
    LOG_DEBUG("new client|ip=%s|port=%u", peerAddr.ip().c_str(), peerAddr.port());
    if (connectionCallback_) connectionCallback_(conn);
    connectionManager_->Add(conn->id(), conn);
    conn->run();
}

void TcpServer::acceptHandler(EventChannel*)
{
    acceptor_.Accept();
}

