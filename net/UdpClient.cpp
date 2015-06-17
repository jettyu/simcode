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
    loop->runInLoop(socket_.sockfd(), SimBind(&UdpClient::eventHandle, this));
    //loop->runInBack();
}

int UdpClient::Send(const char* buf, size_t len)
{
    return conn_->Send(buf, len);
}

void UdpClient::eventHandle()
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
        onMessage(conn_, &buf);
    }
    while (n > 0);
    //queue_.push_back(conn_->id(), SimBind(&UdpClient::onMessage, this, conn_, buf));
}

void UdpClient::onMessage(const UdpConnectionPtr& c, std::string* msg)
{
    messageCallback_(c, msg);
}
