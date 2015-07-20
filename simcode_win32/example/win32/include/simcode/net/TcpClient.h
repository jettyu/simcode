#ifndef WIN32_TCPCLIENT_H
#define WIN32_TCPCLIENT_H

#include <simcode/net/TcpConnection.h>
#include <simcode/net/SockAddr.h>

namespace simcode
{
namespace net
{

class TcpClient
{
public:
	typedef simex::function<void(const TcpConnection::Ptr& conn, Buffer* msg)> MessageCallback;
	typedef simex::function<void(const TcpConnection::Ptr& conn)> ConnectCallback;
	typedef simex::function<void(const TcpConnection::Ptr& conn)> CloseCallback;
	typedef simex::function<void(const TcpConnection::Ptr& conn)> ErrorCallback;
	TcpClient(EventLoop* loop__, const SockAddr& addr, const std::string& name__);
	EventLoop* getLoop()
	{
		return loop_;
	}
	const SockAddr& peerAddr()
	{
		return peerAddr_;
	}
	const std::string& getName() const
	{
		return name_;
	}
	const TcpConnection::Ptr& getConn()
	{
		return conn_;
	}
	bool isActive() const
	{
		return isActived_;
	}
	void setMessageCallback(const MessageCallback& b)
	{
		onMessageCallback_ = b;
	}
	void setConnectionCallback(const ConnectCallback& b)
	{
		onConnectCallback_ = b;
	}
	void setCloseCallback(const CloseCallback& b)
	{
		onCloseCallback_ = b;
	}
	void setErrorCallback(const ErrorCallback& b)
	{
		onErrorCallback_ = b;
	}
	void setRetry(bool retry)
	{
		retry_ = retry;
	}
	void active();
	void close();
	void send(const char* data, size_t size);
	void sendString(const std::string& data);
private:
	void Connect();
	void onConnect(const TcpConnection::Ptr& conn);
	void onClose(const TcpConnection::Ptr& conn);
	void onMessage(const TcpConnection::Ptr& conn, Buffer* msg);
private:
	TcpConnection::Ptr conn_;
	EventLoop* loop_;
	SockAddr peerAddr_;
	std::string name_;
	bool isActived_;
	bool isClosed_;
	bool retry_;
	uint8_t retryTime_;
	typedef simex::shared_ptr<Socket> SocketPtr;
	SocketPtr socketPtr_;
	MessageCallback onMessageCallback_;
	ConnectCallback onConnectCallback_;
	CloseCallback onCloseCallback_;
	ErrorCallback onErrorCallback_;
};
}
}
#endif
