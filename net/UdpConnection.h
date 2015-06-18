#ifndef UDP_CONNECTION_H
#define UDP_CONNECTION_H

/*
 * UdpConnection is not real a socket connection
 * this is just to safe the context of a session
 */
#include <simcode/net/UdpConnector.h>
#include <simcode/thread/thread_safe_queue.h>
#include <simcode/base/noncopyable.h>
namespace simcode
{
namespace net
{

class UdpConnection : noncopyable,
    public simex::enable_shared_from_this<UdpConnection>

{
public:
    typedef SimFunction<void (uint64_t id)> CloseCallback;
    UdpConnection(const UdpConnector& c):
        connector_(c)
    {
    }
    int errcode() const;
    int Send(const char* buf, size_t len);
    int Send(const std::string& data);
    void Close();
    const SockAddr& peerAddr() const;
    uint64_t id() const;

    void setCloseCallback(const CloseCallback& c);

    void setContext(const simex::any& c);
    const simex::any& getContext() const;
    simex::any* getMutableContext();

private:
private:
    UdpConnector connector_;
    simex::any context_;
    CloseCallback closeCallback_;
};

typedef SharedPtr<UdpConnection> UdpConnectionPtr;

inline int UdpConnection::Send(const char* buf, size_t len)
{
    return connector_.Send(buf, len);
}

inline int UdpConnection::Send(const std::string& data)
{
    return connector_.Send(data);
}

inline void UdpConnection::Close()
{
    //connector_.Close();
    if (closeCallback_) closeCallback_(id());
}

inline const SockAddr& UdpConnection::peerAddr() const
{
    return connector_.peerAddr();
}

inline uint64_t UdpConnection::id () const
{
    return peerAddr().id();
}

inline void UdpConnection::setCloseCallback(const CloseCallback& c)
{
    closeCallback_ = c;
}

inline void UdpConnection::setContext(const simex::any& c)
{
    context_ = c;
}

inline const simex::any& UdpConnection::getContext() const
{
    return context_;
}

inline simex::any* UdpConnection::getMutableContext()
{
    return &context_;
}

}
}
#endif
