#include <simcode/net/TcpClient.h>
using namespace simcode;
using namespace net;
TcpClient::TcpClient(EventLoop* loop__, const SockAddr& addr, const std::string& name__) :
	loop_(loop__),
	peerAddr_(addr),
	name_(name__),
	isActived_(false),
	isClosed_(false),
	retry_(false),
	retryTime_(0)
{
}

void TcpClient::active()
{
	isActived_ = true;
	Connect();
}

void TcpClient::close()
{
	isClosed_ = true;
	//retry_ = false;
	conn_.reset();
}

void TcpClient::send(const char* data, size_t size)
{
	TcpConnection::Ptr conn = conn_;
	if (conn) conn_->send(data, size);
}

void TcpClient::sendString(const std::string& data)
{
	TcpConnection::Ptr conn = conn_;
	if (conn) conn_->sendString(data);
}

void TcpClient::Connect()
{
	conn_.reset();
	socketPtr_.reset(new Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
	socketPtr_->setReuseAddr();
	socketPtr_->setReusePort();
	socketPtr_->setTcpNoDelay(true);
	TcpConnection::Ptr conn(new TcpConnection(loop_, socketPtr_->sockfd()));
	conn->setMessageCallback(onMessageCallback_);
	conn->setConnectCallback(simex::bind(&TcpClient::onConnect, this, _1));
	conn->setCloseCallback(simex::bind(&TcpClient::onClose, this, _1));
	conn->setErrorCallback(onErrorCallback_);
	conn->run();
	 
	if (0 == socketPtr_->Connect(peerAddr_))
	{
		conn->setConnected();
	}
	else
	{
	//	if (retry_)
	//	{
	//		Sleep(retryTime_++);
	//		Connect();
	//	}
	}
}

void TcpClient::onConnect(const TcpConnection::Ptr& conn)
{
	if (onConnectCallback_) onConnectCallback_(conn);
	retryTime_ = 0;
}

void TcpClient::onClose(const TcpConnection::Ptr& conn)
{
	if(onCloseCallback_) onCloseCallback_(conn);
	if (retry_)
	{
		Sleep(retryTime_++);
		Connect();
	}
}