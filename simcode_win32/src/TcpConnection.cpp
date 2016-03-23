#include <simcode/net/TcpConnection.h>
#include <simcode/net/EventChannel.h>
static const size_t DEF_HIGHWATERSIZE = 64*1024*1024;
using namespace simcode;
using namespace net;
TcpConnection::TcpConnection(EventLoop* loop__, int fd__, uint64_t id__) :
	loop_(loop__),
    socket_(fd__),
	id_(id__),
    isClosed_(false),
	channel_(new EventChannel(loop__, fd__, simex::bind(&TcpConnection::handle, this, _1))),
    highWaterSize_(DEF_HIGHWATERSIZE),
	isConnected_(false)
{
	socket_.setKeepAlive(true);
	channel_->enableReading();
	channel_->enableWriting();
}

void TcpConnection::send(const char* data, size_t len)
{
    if (!isClosed_)
    {
		{
        ScopeLock lock(mutex_);
        writeBuf_.append(data, len);
        //HighWater
        if (writeBuf_.mutableWriteBuf()->size() > highWaterSize_)
        {
            if (!highWaterCallback_)
            {
                writeBuf_.mutableWriteBuf()->clear();
                return;
            }
            else
            {
                highWaterCallback_(shared_from_this(), &writeBuf_);
                return;
            }
        }
		}
		
		loop_->addTask(simex::bind(&TcpConnection::handleWrite, this));
    }
}

void TcpConnection::sendString(const std::string& data)
{
    if (!isClosed_)
    {
		{
        ScopeLock lock(mutex_);
        writeBuf_.append(data);
        if (writeBuf_.mutableWriteBuf()->size() > highWaterSize_)
        {
            if (!highWaterCallback_)
            {
                writeBuf_.mutableWriteBuf()->clear();
                return;
            }
            else
            {
                highWaterCallback_(shared_from_this(), &writeBuf_);
                return;
            }
        }
		}
		loop_->addTask(simex::bind(&TcpConnection::handleWrite, this));
    }
}

void TcpConnection::handle(EventChannel* ec)
{
	if (ec->isReading()) handleRead();
	if (ec->isWriting()) handleWrite();
	if (ec->isError()) handleError();
}

void TcpConnection::handleRead()
{
    int n;
    char buf[65535];
    n = recv(socket_.sockfd(), buf, sizeof(buf), 0);
    if (n > 0)
    {
        readBuf_.append(buf, n);
        messageCallback_(shared_from_this(), &readBuf_);
    }
    else if (n == 0)
    {
		onClose();
    }
    else
    {
        handleError();
    }
}

void TcpConnection::handleWrite()
{
	if (!isConnected_)
	{
		int error=-1, len; 
		len = sizeof(int);
		getsockopt(socket_.sockfd(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &len); 
		if (error == 0) 
		{
			setConnected();
		}
		return;
	}
    if (writeBuf_.readableBytes() == 0)
    {
        ScopeLock lock(mutex_);
        writeBuf_.changeIndex();
    }
    if (writeBuf_.readableBytes() != 0)
    {
        int fd = socket_.sockfd();
        int n;
		n = ::send(fd, reinterpret_cast<const char*>(writeBuf_.peek()), writeBuf_.readableBytes(), 0);
        if (n > 0)
        {
            if (n < writeBuf_.readableBytes())
            {
				channel_->enableWriting();
            }
            else
            {
                if (writeCompleteCallback_) writeCompleteCallback_(shared_from_this());
            }
        }
        else
        {
            n = 0;
			//printf("write error|errmsg=%s|port=%d", strerror(errno), peerAddr().port());
            if (errno != EWOULDBLOCK)
            {
                //printf("write error|errmsg=%s|port=%d", strerror(errno), peerAddr().port());
                if (errno == EPIPE || errno == ECONNRESET) // FIXME: any others?
                {
                    //shutdown();
					onClose();
                    return;
                }
            }
        }
        writeBuf_.seek(n);
        if (writeBuf_.readableBytes() == 0)
        {
            ScopeLock lock(mutex_);
            writeBuf_.mutableReadBuf()->clear();
            writeBuf_.resetSeek();
			
            if (writeBuf_.mutableWriteBuf()->empty())
            {
				channel_->disableWriting();
            }
            else
            {
                writeBuf_.changeIndex();
            }
        }
    }
}

void TcpConnection::handleError()
{
	if (errorCallback_) 
		errorCallback_(shared_from_this());
	onClose();
}

void TcpConnection::onClose()
{
	if (isClosed_) return;
	isClosed_ = true;
	isConnected_ = false;
	stop();
	if (closeCallback_) 
		closeCallback_(shared_from_this());
}