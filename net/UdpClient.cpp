#include <simcode/net/UdpClient.h>
using namespace simcode;
using namespace net;

UdpClient::UdpClient(EventLoop* loop,
                     const SockAddr& addr,
                     const std::string& name) :
    socket_(AF_INET, SOCK_DGRAM, 0)
{
    assert(socket_.sockfd() != -1);
    socket_.setNonBlockAndCloseOnExec();
    UdpConnector c(socket_.sockfd());
    c.setPeerAddr(addr);
    conn_.reset(new UdpConnection(c));
    channel_.reset(new EventChannel(loop, socket_.sockfd(), simex::bind(&UdpClient::eventHandle, this, _1)));
    channel_->enableReading();
    loop->runInLoop(channel_);
    //loop->runInBack();
}

int UdpClient::send(const char* buf, size_t len)
{
    return conn_->send(buf, len);
}

int UdpClient::sendString(const std::string& data)
{
    return conn_->sendString(data);
}

void UdpClient::eventHandle(EventChannel*)
{
    UdpConnector c(socket_.sockfd());
    int n = 0;
    char tmp[65535];
    if ( (n=c.recv(tmp, 65535)) < 0)
    {
        return;
    }
    messageCallback_(conn_, tmp, n);
    //queue_.push_back(conn_->id(), SimBind(&UdpClient::onMessage, this, conn_, buf));
}

