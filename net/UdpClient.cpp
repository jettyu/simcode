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

int UdpClient::Send(const char* buf, size_t len)
{
    return conn_->Send(buf, len);
}

int UdpClient::SendString(const std::string& data)
{
    return conn_->SendString(data);
}

void UdpClient::eventHandle(EventChannel*)
{
    UdpConnector c(socket_.sockfd());
    int n = 0;
    do
    {
        std::string buf;
        if ( (n=c.Recv(&buf)) < 0)
        {
            return;
        }
        onMessage(conn_, buf);
    }
    while (n > 0);
    //queue_.push_back(conn_->id(), SimBind(&UdpClient::onMessage, this, conn_, buf));
}

void UdpClient::onMessage(const UdpConnectionPtr& c, const std::string& msg)
{
    messageCallback_(c, msg);
}
