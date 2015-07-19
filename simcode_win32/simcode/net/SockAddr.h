#ifndef SN_SOCKADDR_H
#define SN_SOCKADDR_H
#include <stdint.h>
#include <simcode/base/copyable.h>
#include <string>
#ifdef WIN32
#include <winsock2.h>
#endif
typedef struct sockaddr_in sockaddr_in_t;
namespace simcode
{
namespace net
{

class SockAddr : copyable
{
public:
    /** Default constructor */
    SockAddr(const std::string& ip, uint16_t port)
    {
        Init(ip, port);
    }
    SockAddr(const ::sockaddr_in_t& a)
    {
        addr_in_ = a;
    }
    /** Default destructor */
    virtual ~SockAddr() {}
    int Init(const std::string& ip__, uint16_t port__);

    uint16_t port() const
    {
        return ntohs(addr_in_.sin_port);
    }

    uint64_t id() const
    {
        return (addr_in_.sin_addr.s_addr << 16) | addr_in_.sin_port;
    }

    void set_addr_in(const ::sockaddr_in_t& a)
    {
        addr_in_ = a;
    }
    const struct sockaddr* addr() const
    {
        return reinterpret_cast<const struct sockaddr*>(&addr_in_);
    }
    const struct sockaddr_in* addr_in() const
    {
        return &addr_in_;
    }
protected:
private:
    sockaddr_in_t addr_in_;
};

static void bzero(void* dst, size_t len)
{
    memset(dst, 0, len);
}

inline int SockAddr::Init(const std::string& ip__, uint16_t port__)
{
    bzero(&addr_in_, sizeof addr_in_);
    addr_in_.sin_family = AF_INET;
    addr_in_.sin_port = htons(port__);
    addr_in_.sin_addr.S_un.S_addr = inet_addr(ip__.c_str());
    return 0;
    //return ::inet_pton(AF_INET, ip__.c_str(), &addr_in_.sin_addr);
}

}
}
#endif // SN_SOCKADDR_H
