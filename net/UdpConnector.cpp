#include <simcode/net/UdpConnector.h>
#include <simcode/net/NetLogger.h>
using namespace simcode;
using namespace net;

UdpConnector::UdpConnector(int sockfd__):
    sockfd_(sockfd__),
    addrLen_(sizeof(struct sockaddr_in))
{
}
#define MAX_BUF_LEN 65535
int UdpConnector::Recv(std::string* buf)
{
    struct sockaddr_in addr;
    int n = 0;
    char tmpbuf[MAX_BUF_LEN];
    n = ::recvfrom(sockfd_, tmpbuf, MAX_BUF_LEN, 0, (struct sockaddr*)&addr, &addrLen_);
    if (n > 0) buf->append(tmpbuf, n);
    errcode_ = errno;
    peerAddr_.set_addr_in(addr);
    return n;
}

int UdpConnector::Send(const std::string& buf)
{
    return Send(buf.data(), buf.size());
}

int UdpConnector::Send(const char* buf, size_t len)
{
    int ret = sendto(sockfd_, buf, len, 0, peerAddr_.addr(), addrLen_);
    errcode_ = errno;
    NETLOG_ERROR("write error|errno=%d|errmsg=%s", errcode_, strerror(errcode_));
    return ret;
}
