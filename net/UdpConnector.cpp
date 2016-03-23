#include <simcode/net/UdpConnector.h>
#include <simcode/base/logger.h>
using namespace simcode;
using namespace net;

UdpConnector::UdpConnector(int sockfd__):
    sockfd_(sockfd__),
    addrLen_(sizeof(struct sockaddr_in))
{
}
#define MAX_BUF_LEN 65535
int UdpConnector::recv(char* buf, size_t size)
{
    struct sockaddr_in addr;
    int n = 0;
    n = ::recvfrom(sockfd_, buf, size, 0, (struct sockaddr*)&addr, &addrLen_);
    errcode_ = errno;
    peerAddr_.set_addr_in(addr);
    return n;
}

int UdpConnector::sendString(const std::string& buf)
{
    return send(buf.data(), buf.size());
}

int UdpConnector::send(const char* buf, size_t len)
{
    int ret = sendto(sockfd_, buf, len, 0, peerAddr_.addr(), addrLen_);
    errcode_ = errno;
    if (ret < 0)
    {
        LOG_ERROR("write error|errno=%d|errmsg=%s", errcode_, strerror(errcode_));
    }

    return ret;
}
