#include <simcode/librtmp/RtmpTrans.h>

using namespace simcode;
using namespace rtmp;

class RecvHandle : public RtmpRecvCallback
{
public:
    RecvHandle(RtmpTrans* r):trans_(r) {}
    void run(const char* buf, int size)
    {
        trans_->handleRead(buf, size);
    }
private:
    RtmpTrans* trans_;
};

class RecvHandleAsync : public RtmpRecvCallback
{
public:
    RecvHandleAsync(RtmpTrans* r):trans_(r) {}
    void run(const char* buf, int size)
    {
        trans_->handleReadAsync(buf, size);
    }
private:
    RtmpTrans* trans_;
};

void RtmpTrans::pull_push()
{
    std::string buf;
    RtmpRecvCallback* callback = new RecvHandle(this);
    pull_conn_->set_recv_callback(callback);
    while (1)
    {
        buf.clear();
        buf.resize(1024*64);
        if (!pull_conn_->handleRead())
        {
            sleep(pull_reconn_time_++);
            pull_conn_->Connect();
        }
        else
        {
            pull_reconn_time_ = 0;
            /* n = push_conn_->Write(buf.data(), buf.size());
            if (n < 0)
            {
            	sleep(push_reconn_time_++);
            	push_conn_->Connect();
            	push_conn_->EnableWrite();
            	push_conn_->ConnectStream(0);
            }
            else
            {
            	push_reconn_time_ = 0;
            } */
        }
    }
    delete callback;
}

void RtmpTrans::pull_while()
{
    std::string buf;
    buf.resize(1024*64);
    RtmpRecvCallback* callback = new RecvHandleAsync(this);
    pull_conn_->set_recv_callback(callback);
    while (1)
    {
        if (!pull_conn_->handleRead())
        {
            sleep(pull_reconn_time_++);
            pull_conn_->Connect();
        }
        else
        {
            pull_reconn_time_ = 0;
            //ScopeLock lock(mutex_);
        }
    }
}
void RtmpTrans::push_while()
{
    while (1)
    {
        int n = 0;
        {
            //ScopeLock lock(mutex_);
            if (trans_buf_.empty())
            {
                usleep(1);
                continue;
            }
            n = push_conn_->Write(trans_buf_.data(), trans_buf_.size());
            if (n > 0)
            {
                push_reconn_time_ = 0;
                trans_buf_ = std::string(trans_buf_.data()+n, trans_buf_.size()-n);
                continue;
            }
        }
        if (n < 0)
        {
            sleep(push_reconn_time_++);
            push_conn_->Connect();
            push_conn_->EnableWrite();
            push_conn_->ConnectStream(0);
            continue;
        }
    }
}

void RtmpTrans::handleRead(const char* buf, int size)
{
    /* n = push_conn_->Write(buf.data(), buf.size());
    if (n < 0)
    {
    	sleep(push_reconn_time_++);
    	push_conn_->Connect();
    	push_conn_->EnableWrite();
    	push_conn_->ConnectStream(0);
    }
    else
    {
    	push_reconn_time_ = 0;
    } */
}

void RtmpTrans::handleReadAsync(const char* buf, int size)
{
    /*
    ScopeLock lock(mutex_);
    trans_buf_.append(buf, size);
     */
}

