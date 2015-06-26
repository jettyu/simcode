#include <simcode/net/TcpConnection.h>
#include <simcode/base/logger.h>
using namespace simcode;
using namespace net;
static const int DEF_HIGHWATERSIZE = 64*1024*1024;
TcpConnection::TcpConnection(EventLoop* loop, int connfd, const SockAddr& peerAddr):
    loop_(loop),
    socket_(connfd),
    peerAddr_(peerAddr),
    localAddr_(SockAddr(socket_.getLocalAddr())),
    errcode_(0),
    events_(EPOLLIN|EPOLLPRI),
    revents_(0),
	highWaterSize_(DEF_HIGHWATERSIZE),
    isClosed_(false)
{
    socket_.setKeepAlive(true);
    //readBuf_.reserve(10240);
    //writeBuf_.mutableReadBuf()->reserve(10240);
    //writeBuf_.mutableWriteBuf()->reserve(10240);
}

void TcpConnection::run()
{
    loop_->runInLoop(socket_.sockfd(), simex::bind(&TcpConnection::eventHandle, this, _1), events_);
}

void TcpConnection::eventHandle(int events)
{
    revents_ = events;
    if (isReading())
    {
        handleRead();
    }
    if (isWriting())
    {
        handleWrite();
    }
}

void TcpConnection::handleRead()
{
    // saved an ioctl()/FIONREAD call to tell how much to read
    int fd = socket_.sockfd();
    const size_t writable = 65536;
    char tmpbuf[65536];
    char extrabuf[65536];
    struct iovec vec[2];
    vec[0].iov_base = tmpbuf;
    vec[0].iov_len = sizeof tmpbuf;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    // when there is enough space in this buffer, don't read into extrabuf.
    // when extrabuf is used, we read 128k-1 bytes at most.
    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);
    if (n < 0)
    {
        errcode_ = errno;
        LOG_ERROR("read error|errno=%d|errmsg=%s", errcode_, strerror(errcode_));
        onClose();
        return;
    }
    else if (n == 0)
    {
        onClose();
        return;
    }
    else if (static_cast<size_t>(n) <= writable)
    {
        readBuf_.append(tmpbuf, n);
    }
    else
    {
        readBuf_.append(tmpbuf, sizeof tmpbuf);
        readBuf_.append(extrabuf, sizeof extrabuf);
    }
    messageCallback_(shared_from_this(), &readBuf_);
}

void TcpConnection::handleWrite()
{
   // if (isClosed_) return;
    if (writeBuf_.readableBytes() == 0)
    {
        ScopeLock lock(mutex_);
        writeBuf_.changeIndex();
    }
    if (writeBuf_.readableBytes() != 0)
    {
        int fd = socket_.sockfd();
        ssize_t n;
        n = ::write(fd, writeBuf_.peek(), writeBuf_.readableBytes());
        if (n > 0)
        {
            if (n < writeBuf_.readableBytes())
            {

            }
            else
            {
            }
        }
        else
        {
            n = 0;
            int e = errno;
            errcode_ = e;

            disableWriting();
            if (errno != EWOULDBLOCK)
            {
                LOG_DEBUG("write error|errmsg=%s|port=%d", strerror(errno), peerAddr().port());
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
                disableWriting();
            }
            else
            {
                writeBuf_.changeIndex();
            }
        }
    }

}

void TcpConnection::send(const char* data, size_t len)
{
    if (!isClosed_)
    {
        ScopeLock lock(mutex_);
		//HighWater
		/*if (writeBuf_.mutableWriteBuf()->size() > highWaterSize_)
		{
			if (!highWaterCallback_) writeBuf_.clear();
			else if (!highWaterCallback_(shared_from_this(), &writeBuf_)) return;
		}
		 */
        writeBuf_.append(data, len);
        enableWriting();
    }
    //handleWrite();
}

void TcpConnection::send(const std::string& data)
{
    if (!isClosed_)
    {
        ScopeLock lock(mutex_);
        writeBuf_.append(data);
        enableWriting();
    }
}

void TcpConnection::update()
{
    loop_->modifyEvent(socket_.sockfd(), events_);
}

void TcpConnection::onClose()
{
    isClosed_ = true;
    //shutdown();

    loop_->removeInLoop(socket_.sockfd());
    if (closeCallback_) closeCallback_();
}
