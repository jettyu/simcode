#include <simcode/net/Connector.h>
using namespace simcode;
using namespace simcode::net;

void Connector::Connect()
{
    socket_.reset(new Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP));
    socket_->setReuseAddr(true);
    socket_->setReusePort(true);
    socket_->setNonBlockAndCloseOnExec();
    TryConnect();    
}

void Connector::TryConnect()
{
    int ret = socket_->Connect(connAddr_);
    int savedErrno = (ret == 0) ? 0 : errno;
    int flag = 0;
    switch (savedErrno)
    {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            flag = 0;
            break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            flag = 1;
            //retry(sockfd);
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            //LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
            socket_.reset();
            flag = 2;
            break;

        default:
            //LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
            socket_.reset();
            flag = 2;
            // connectErrorCallback_();
            break;
    }
    if (0 == flag)
    {
        setState(kConnecting);
        handleWrite();
    }
    else if (1 == flag)
    {
        loop_->runAfter(retryTime_++, simex::bind(&Connector::TryConnect, this));
    }
    else if (2 == flag)
    {
        loop_->runAfter(retryTime_++, simex::bind(&Connector::Connect, this));
    }
}

void Connector::handleWrite()
{
    if (state_ == kConnecting)
    {
        int err = socket_->getError();
        if (err)
        {
            TryConnect();
        }
        else if (socket_->isSelfConnect())
        {
            //LOG_WARN << "Connector::handleWrite - Self connect";
            TryConnect();
        }
        else
        {
            setState(kConnected);
            retryTime_ = 0;
            if (newConnectionCallback_) newConnectionCallback_();
        }
    }
    else
    {
        // what happened?
        //assert(state_ == kDisconnected);
    }
}
