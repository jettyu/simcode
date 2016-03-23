#ifndef SN_CONNECTOR_H
#define SN_CONNECTOR_H

#include <simcode/net/CommonHead.h>
#include <simcode/net/SockAddr.h>
#include <simcode/net/Socket.h>
#include <simcode/base/typedef.h>
#include <simcode/base/noncopyable.h>
#include <simcode/net/EventLoop.h>
namespace simcode
{
namespace net
{
typedef SharedPtr<Socket> SocketPtr;
class Connector : noncopyable
{
public:
    typedef simex::function<void()> NewConnectionCallback;
    /** Default constructor */
    Connector(EventLoop* loop,const SockAddr& _connAddr) :
        loop_(loop),
        connAddr_(_connAddr),
        retryTime_(0),
        state_(kDisconnected)
    {
    }
    void setNewConnectionCallback(const NewConnectionCallback& b)
    {
        newConnectionCallback_ = b;
    }
    void Connect();
    const SocketPtr& getSocket() const
    {
        return socket_;
    }
    const SockAddr& connAddr() const
    {
        return connAddr_;
    }
protected:
private:
    enum States { kDisconnected, kConnecting, kConnected };
    void setState(States s)
    {
        state_ = s;
    }
    EventLoop* loop_;
    NewConnectionCallback newConnectionCallback_;
    void TryConnect();
    void handleWrite();
    SocketPtr socket_;
    SockAddr connAddr_;
    uint8_t retryTime_;
    States state_;
};

}
}

#endif // SN_CONNECTOR_H
