#ifndef SN_ACCEPTOR_H
#define SN_ACCEPTOR_H
#include <simcode/net/Socket.h>
#include <simcode/base/typedef.h>
#include <simcode/base/noncopyable.h>
namespace simcode
{
namespace net
{
class Acceptor : noncopyable
{
public:
    typedef SimFunction<void(int sockfd, const SockAddr&)> NewConnectionCallback;
    /** Default constructor */
    Acceptor(const SockAddr& listenAddr, bool reuseport = false);

    /** Default destructor */
    virtual ~Acceptor();
    void setCallback(const NewConnectionCallback& c)
    {
        newConnectionCallback_ = c;
    }
    bool IsListening()
    {
        return isListening_;
    }
    int sockfd() const
    {
        return socket_.sockfd();
    }
    int Listen(int backlog = 1024);
    int Accept();
protected:
private:
    Socket socket_;
    bool isListening_;
    int idleFd_;
    NewConnectionCallback newConnectionCallback_;
};
}
}
#endif // SN_ACCEPTOR_H
