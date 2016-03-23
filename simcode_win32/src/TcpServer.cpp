#include <simcode/net/TcpServer.h>
#include <simcode/base/logger.h>
using namespace simcode;
using namespace net;

TcpServer::TcpServer(EventLoop* loop, const SockAddr& addr, const std::string&name, bool reuseport):
    loop_(loop),
    acceptor_(addr, reuseport),
    threadNum_(0),
	connid_(0)
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

	loop_->addChannel(acceptChannel_);
}

void TcpServer::onClose(const TcpConnectionPtr& conn)
{
    if (closeCallback_) closeCallback_(conn);
    EventLoop* ioLoop = conn->getLoop();
	ioLoop->removeChannel(conn->fd());
	loop_->addTask(simex::bind(&TcpServer::connectListErase, this, conn->id()));
}

void TcpServer::onConnection(int connfd, const SockAddr& peerAddr)
{
    EventLoop* ioLoop = loopThreadPool_.getNextLoop();
    if (!ioLoop) ioLoop = loop_;
	TcpConnectionPtr conn(new TcpConnection(ioLoop, connfd, ++connid_));
    conn->setCloseCallback(simex::bind(&TcpServer::onClose, this, _1));
    conn->setMessageCallback(messageCallback_);
    if (connectionCallback_) connectionCallback_(conn);
    conn->run();
	loop_->addTask(simex::bind(&TcpServer::connectListAdd, this, conn));
}

void TcpServer::acceptHandler(EventChannel*)
{
    acceptor_.Accept();
}

void TcpServer::connectListErase(uint64_t id)
{
	connectionList_.erase(id);
}

void TcpServer::connectListAdd(const TcpConnectionPtr& conn)
{
	connectionList_[conn->id()] = conn;
}