#ifndef WIN_TCPCONNECTION_H
#define WIN_TCPCONNECTION_H

#include <simcode/base/noncopyable.h>
#include <simcode/base/typedef.h>
#include <simcode/net/Buffer.h>
#include <simcode/net/OutBuffer.h>

#include <simcode/net/Selector.h>
#include <simcode/net/Socket.h>
#include <simcode/net/EventLoop.h>

namespace simcode
{
namespace net
{
class TcpConnection : noncopyable,
    public simex::enable_shared_from_this<TcpConnection>
{
public:
    typedef simex::shared_ptr<TcpConnection> Ptr;
    typedef simex::function<void(const Ptr&)> CloseCallback;
    typedef simex::function<void(const Ptr&, Buffer* msg)> MessageCallback;
    typedef simex::function<bool(const Ptr&, OutBuffer* buf)> HighWaterCallback;
    typedef simex::function<void(const Ptr&)> WriteCompleteCallback;
	typedef simex::function<void(const Ptr&)> ErrorCallback;
    TcpConnection(EventLoop* loop__, int fd__);
    ~TcpConnection()
    {
		onClose();
    }
	int fd() const
	{
		return socket_.sockfd();
	}
    void run()
    {
        loop_->addChannel(channel_);
    }
    void stop()
    {
        loop_->removeChannel(socket_.sockfd());
    }
	EventLoop* getLoop()
	{
		return loop_;
	}
    void setCloseCallback(const CloseCallback& b)
    {
        closeCallback_ = b;
    }
    void setMessageCallback(const MessageCallback& b)
    {
        messageCallback_ = b;
    }
	void setErrorCallback(const ErrorCallback& b)
	{
		errorCallback_ = b;
	}

	void setContext(const simex::any& c)
	{
		context_ = c;
	}

	const simex::any& getContext() const
	{
		return context_;
	}
	simex::any* getMutableContext()
	{
		return &context_;
	}

    void send(const char* data, size_t len);

    void sendString(const std::string& data);

private:
	void handle(EventChannel*);
    void handleRead();
    void handleWrite();
    void handleError();
	void onClose();
private:
	EventLoop* loop_;
    Socket socket_;
    bool isClosed_;
	simex::shared_ptr<EventChannel> channel_;
    Buffer readBuf_;
    OutBuffer writeBuf_;
    Mutex mutex_;
    CloseCallback closeCallback_;
    MessageCallback messageCallback_;
    HighWaterCallback highWaterCallback_;
    WriteCompleteCallback writeCompleteCallback_;
	ErrorCallback errorCallback_;
    size_t highWaterSize_;
	simex::any context_;
};

}
}
#endif // WIN_TCPCONNECTION_H
