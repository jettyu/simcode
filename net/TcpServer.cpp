#include <simcode/net/TcpServer.h>
#include <simcode/base/logger.h>
using namespace simcode;
using namespace net;

TcpServer::TcpServer(EventLoop* loop,
                     const SockAddr& addr,
                     const std::string&name,
                     bool reuseport):
    loop_(loop),
    acceptor_(addr, reuseport),
    threadNum_(0),
    connectionId_(0),
    sendLoop_(loop)
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
    LOG_TRACE("ret=%d", ret);
    assert(ret == 0);

    connectionManager_.reset(new TcpConnectionManager(sendLoop_, simex::bind(&TcpServer::send, this, _1,_2)));

    loop_->runInLoop(acceptChannel_);
}

static void stopAllConn(std::map<uint64_t, TcpConnectionPtr>& connList)
{
    std::map<uint64_t, TcpConnectionPtr>::iterator it;
    for (it=connList.begin(); it!=connList.end(); ++it)
    {
        it->second->shutdownRead();
    }
}

void TcpServer::stop()
{
    loop_->removeInLoop(acceptChannel_->fd());
    connectionManager_->AddCallback(simex::bind(stopAllConn, _1));
}

void TcpServer::onClose(const TcpConnectionPtr& conn)
{
    if (closeCallback_) closeCallback_(conn);
//    EventLoop* ioLoop = conn->getLoop();
  //  ioLoop->removeInLoop(conn->connfd());
    LOG_TRACE("client close|ip=%s|port=%u", conn->peerAddr().ip().c_str(), conn->peerAddr().port());
    connectionManager_->Remove(conn->id());
}

void TcpServer::onConnection(int connfd, const SockAddr& peerAddr)
{
    EventLoop* ioLoop = loopThreadPool_.getNextLoop();
    if (!ioLoop) ioLoop = loop_;
    TcpConnectionPtr conn(new TcpConnection(ioLoop, connfd, peerAddr, ++connectionId_));
    conn->setCloseCallback(simex::bind(&TcpServer::onClose, this, _1));
    conn->setMessageCallback(messageCallback_);
    LOG_TRACE("new client|ip=%s|port=%u", peerAddr.ip().c_str(), peerAddr.port());
    connectionManager_->Add(conn->id(), conn);
    conn->run();
    if (connectionCallback_) connectionCallback_(conn);
}

void TcpServer::acceptHandler(EventChannel*)
{
    acceptor_.Accept();
}

void TcpServer::send(const TcpConnectionPtr& conn, const std::string& data)
{
    conn->sendString(data);
}

