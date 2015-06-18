#include <simcode/net/TcpClient.h>
#include <iostream>
using namespace std;
using namespace simcode;
using namespace net;

TcpClient::TcpClient(EventLoop* loop, const SockAddr& addr, const std::string& name):
    loop_(loop),
    connector_(loop, addr),
    retry_(true)
{
    connector_.setNewConnectionCallback(simex::bind(&TcpClient::onConnect, this));
}

TcpClient::~TcpClient()
{
    if (conn_) loop_->removeInLoop(conn_->connfd());
}

void TcpClient::active()
{
    connector_.Connect();
}

void TcpClient::send(const char* data, size_t len)
{
    TcpConnectionPtr conn;
    {
        ScopeLock lock(mutex_);
        conn = conn_;
    }
    if (conn) conn->send(data, len);
}

void TcpClient::send(const std::string& data)
{
    TcpConnectionPtr conn;
    {
        ScopeLock lock(mutex_);
        conn = conn_;
    }
    if (conn) conn->send(data);
}

void TcpClient::onConnect()
{
    TcpConnectionPtr newConn(new TcpConnection(loop_, connector_.getSocket()->sockfd(), connector_.connAddr()));
    newConn->setCloseCallback(simex::bind(&TcpClient::onClose, this, conn_));
    newConn->setMessageCallback(messageCallback_);
    conn_ = newConn;
    conn_->run();
    if (connectionCallback_) connectionCallback_(conn_);
}

void TcpClient::onClose(const TcpConnectionPtr& conn)
{
    if (closeCallback_) closeCallback_(conn);
    if (retry_) connector_.Connect();
}
