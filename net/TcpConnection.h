#ifndef SIMCODE_NET_TCPCONNECTION_H
#define SIMCODE_NET_TCPCONNECTION_H
#include <simcode/net/SockAddr.h>
#include <simcode/net/Socket.h>
#include <simcode/net/EventLoop.h>
#include <simcode/base/typedef.h>
#include <simcode/base/noncopyable.h>
#include <simcode/net/Buffer.h>
#include <simcode/net/OutBuffer.h>
namespace simcode
{
namespace net
{
class TcpConnection;
typedef simex::shared_ptr<TcpConnection> TcpConnectionPtr;
class TcpConnection : noncopyable,
    public simex::enable_shared_from_this<TcpConnection>
{
public:
    typedef simex::function<void(const TcpConnectionPtr&)> CloseCallback;
    typedef simex::function<void(const TcpConnectionPtr&, Buffer* msg)> MessageCallback;
	typedef simex::function<bool(const TcpConnectionPtr&, OutBuffer* buf)> HighWaterCallback;
	typedef simex::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
	typedef void(TcpConnection::*send_buf_t)(const char*, size_t);
	typedef void(TcpConnection::*send_str_t)(const std::string&);
    TcpConnection(EventLoop* loop, int connfd, const SockAddr& peerAddr);
    ~TcpConnection();
    void run();
    void send(const char* data, size_t len);
    void sendString(const std::string& data);
    int connfd() const
    {
        return socket_.sockfd();
    }
    const SockAddr& peerAddr() const
    {
        return peerAddr_;
    }
    const SockAddr& localAddr() const
    {
        return localAddr_;
    }
    EventLoop* getLoop() const
    {
        return loop_;
    }
    void Close()
    {
        onClose();
    }
    void setCloseCallback(const CloseCallback& b)
    {
        closeCallback_ = b;
    }
    void setMessageCallback(const MessageCallback& b)
    {
        messageCallback_ = b;
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
    int errcode() const
    {
        return errcode_;
    }
    bool isWriting() const
    {
        return events_ & EPOLLOUT;
    }
    bool isReading() const
    {
        return revents_ & EPOLLIN;
    }
    void setEvenst(int events)
    {
        revents_ = events;
    }
    void shutdown()
    {
        isClosed_ = true;
        socket_.ShutdownWrite();
    }
	void setHighWaterCallback(const HighWaterCallback& c)
	{
		highWaterCallback_ = c;
	}
	void setHighWaterSize(int n)
	{
		highWaterSize_ = n;
	}
	void setWriteCompleteCallback(const WriteCompleteCallback& c)
	{
		writeCompleteCallback_ = c;
	}
private:
    void eventHandle(int events);
    void onClose();
    void enableWriting()
    {
        events_ |= EPOLLOUT;
        update();
    }
    void disableWriting()
    {
        events_ &= ~EPOLLOUT;
        update();
    }
    void handleRead();
    void handleWrite();
    void update();
private:
    EventLoop* loop_;
    Socket socket_;
    const SockAddr peerAddr_;
    const SockAddr localAddr_;
    CloseCallback closeCallback_;
    MessageCallback messageCallback_;
	HighWaterCallback highWaterCallback_;
	WriteCompleteCallback writeCompleteCallback_;
    Buffer readBuf_;
    OutBuffer writeBuf_;
    int errcode_; //use to save errno
    int events_;
    int revents_;
	int highWaterSize_;
    bool isClosed_;
    Mutex mutex_;
    simex::any context_;
};

typedef simex::shared_ptr<TcpConnection> TcpConnectionPtr;
}
}

#endif
