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
	if (0 == socketPtr_->Connect(peerAddr_))
	{
		retryTime_ = 0;
		TcpConnection::Ptr conn(new TcpConnection(loop_, socketPtr_->sockfd()));
		conn->setMessageCallback(onMessageCallback_);
		conn->setCloseCallback(simex::bind(&TcpClient::onClose, this, _1));
		conn->setErrorCallback(onErrorCallback_);
		if (onConnectCallback_) onConnectCallback_(conn);
		conn->run();
		conn_ = conn;
	}
	else
	{
		if (retry_)
		{
			Sleep(retryTime_++);
			Connect();
		}
	}
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