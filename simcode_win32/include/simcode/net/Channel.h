#ifndef CHANNEL_H
#define CHANNEL_H
#include <simcode/base/typedef.h>

namespace simcode
{
namespace net
{

class Channel
{
public:
    typedef simex::function<void()> Callback;
    Channel(int fd__):fd_(fd__){}
    int fd() const
    {
        return fd_;
    }

    void set_readCallback(const Callback& b)
    {
        readCallback_ = b;
    }
    void set_writeCallback(const Callback& b)
    {
        writeCallback_ = b;
    }
    void set_errorCallback(const Callback& b)
    {
        errorCallback_ = b;
    }

    void handleRead()
    {
        readCallback_();
    }
    void handleWrite()
    {
        writeCallback_();
    }
    void handleError()
    {
        if (errorCallback_) errorCallback_();
    }
private:
    int fd_;
    Callback readCallback_;
    Callback writeCallback_;
    Callback errorCallback_;
};

}
}
#endif // CHANNEL_H
