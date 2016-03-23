#ifndef RTMP_CONN_H
#define RTMP_CONN_H

#include <librtmp/rtmp_sys.h>
#include <librtmp/log.h>
#include <string>
#include <simcode/librtmp/RtmpPacket.h>
namespace simcode
{
namespace rtmp
{

class RtmpRecvCallback
{
public:
    virtual void run(const char* buf, int size) = 0;
};

class RtmpConn
{
public:
    RtmpConn() : rtmp_(NULL), timeout_(30), recv_callback_(NULL)
    {}
    ~RtmpConn()
    {
        Free();
    }
    void set_url(const std::string& url)
    {
        url_ = url;
    }
    void set_timeout(int t)
    {
        timeout_ = t;
    }
    void set_recv_callback(RtmpRecvCallback* c)
    {
        recv_callback_ = c;
    }
    void Free()
    {
        if (rtmp_)
        {
            RTMP_Close(rtmp_);//断开连接
            RTMP_Free(rtmp_);//释放内存
            rtmp_ = NULL;
        }
    }
    void InitRead()
    {
        Init();
        rtmp_->m_read.flags |= RTMP_READ_RESUME;
        //read_duration_ = RTMP_GetDuration(rtmp_);
    }
    void EnableWrite()
    {
        Init();
        RTMP_EnableWrite(rtmp_);//设置可写状态
    }
    int Connect()
    {
        //Free();
        Init();
        return RTMP_Connect(rtmp_,NULL);
    }
    int IsConnect()
    {
        rtmp_!=NULL && RTMP_IsConnected(rtmp_);
    }
    //创建流
    int ConnectStream(int linkFlag)
    {
        return RTMP_ConnectStream(rtmp_, linkFlag);
    }
    int StreamId() const
    {
        return rtmp_->m_stream_id;
    }
    int SendPacket(CRtmpPacket& packet)
    {
        if (IsConnect()) return RTMP_SendPacket(rtmp_, packet.getPacket(), 0);
        else return 0;
    }
    int Read(char *buf, int size)
    {
        return RTMP_Read(rtmp_, buf, size);
    }

    int Write(const char* buf, int size)
    {
        return RTMP_Write(rtmp_, buf, size);
    }

    bool handleRead();
private:
    void Init()
    {
        if (rtmp_ == NULL)
        {
            rtmp_ = RTMP_Alloc();
            RTMP_Init(rtmp_);
            rtmp_->Link.timeout=timeout_;
            int ret = RTMP_SetupURL(rtmp_, (char*)url_.data());//设置url
        }
    }

private:
    RTMP* rtmp_;
    RtmpRecvCallback* recv_callback_;
    std::string url_;
    uint32_t timeout_;
    //double read_duration_;
};

}
}

#endif // CLASS_RTMP_SOCKET_H
