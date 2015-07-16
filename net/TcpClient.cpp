#include <simcode/net/TcpClient.h>
#include <simcode/base/logger.h>
using namespace simcode;
using namespace net;

TcpClient::TcpClient(EventLoop* loop, const SockAddr& addr, const std::string& name):
    loop_(loop),
    connector_(loop, addr),
    retry_(true),
    isActive_(false)
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
    isActive_ = true;
}

void TcpClient::send(const char* data, size_t len)
{
    TcpConnectionPtr conn = conn_;
    if (conn) conn->send(data, len);
}

void TcpClient::sendString(const std::string& data)
{
    TcpConnectionPtr conn = conn_;
    if (conn) conn->sendString(data);
}
void TcpClient::Close()
{
    TcpConnectionPtr conn = conn_;
    retry_ = false;
    if (conn) conn->Close();
}
void TcpClient::onConnect()
{
    TcpConnectionPtr newConn(new TcpConnection(loop_, connector_.getSocket()->sockfd(), connector_.connAddr()));
    newConn->setCloseCallback(simex::bind(&TcpClient::onClose, this, _1));
    newConn->setMessageCallback(messageCallback_);
    conn_ = newConn;
    conn_->run();
    LOG_DEBUG("new connection ip=%s|port=%d", conn_->peerAddr().ip().c_str(), conn_->peerAddr().port());
    if (connectionCallback_) connectionCallback_(conn_);
}

void TcpClient::onClose(const TcpConnectionPtr& conn)
{
    LOG_DEBUG("close connection ip=%s|port=%d", conn_->peerAddr().ip().c_str(), conn_->peerAddr().port());
    if (closeCallback_) closeCallback_(conn);
	conn_->Close();
	conn_.reset();
    if (retry_) connector_.Connect();
}
