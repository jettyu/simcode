#ifndef RTMP_TRANS_H
#define RTMP_TRANS_H

#include <simcode/librtmp/RtmpConn.h>
//include <simcode/base/typedef.h>

namespace simcode
{
namespace rtmp
{

class RtmpTrans
{
public:
    RtmpTrans():pull_conn_(NULL), push_conn_(NULL), pull_reconn_time_(1), push_reconn_time_(1)
    {}
    ~RtmpTrans() {}
    void set_pull_conn(RtmpConn* c)
    {
        pull_conn_ = c;
    }
    void set_push_conn(RtmpConn* c)
    {
        push_conn_ = c;
    }
    void pull_push();
    void pull_while();
    void push_while();
    void handleRead(const char* buf, int size);
    void handleReadAsync(const char* buf, int size);
private:
    RtmpConn* pull_conn_;
    RtmpConn* push_conn_;
    std::string trans_buf_;
    int pull_reconn_time_;
    int push_reconn_time_;
    //Mutex mutex_;
};


}
}

#endif
