#ifndef SIM_REDIS_H_
#define SIM_REDIS_H_
#include <iostream>
#include <hiredis/hiredis.h>
#include <vector>
#include <string.h>

namespace simcode
{
namespace redis
{

struct RedisInfo
{
    std::string host;
    uint32_t port;
    struct timeval time_out;
    bool operator < (const RedisInfo& info) const
    {
        if (host != info.host) return host < info.host;
        else return port < info.port;
    }
    void swap(RedisInfo& info)
    {
        host.swap(info.host);
        std::swap(port, info.port);
        std::swap(time_out, info.time_out);
    }
};

class RedisReply
{
public:
    RedisReply(redisReply* r):reply_(r) {}
    ~RedisReply()
    {
        Free();
    }
    redisReply* get(void) const
    {
        return reply_;
    }
    redisReply* operator->() const
    {
        return reply_;
    }
    RedisReply& operator=(redisReply* r)
    {
        if (r != reply_)
        {
            Free();
            reply_ = r;
        }
        return *this;
    }
    operator const bool ()
    {
        return reply_!=NULL;
    }
    void Free(void)
    {
        if (reply_) freeReplyObject(reply_);
        reply_ = NULL;
    }
private:
    redisReply* reply_;
};

class Redis
{
public:
    Redis():m_errcode(0),m_ctxt(NULL) {}
    Redis(const Redis& h):m_errcode(0),m_ctxt(NULL)
    {
        *this = h;
    }
    Redis(const RedisInfo& _info):m_errcode(0),m_ctxt(NULL)
    {
        Reset(_info);
    }
    Redis(const std::string& host,
          const int port,
          const struct timeval& time_out)
        :m_errcode(0), m_ctxt(NULL)
    {
        Reset(host, port, time_out);
    }
    ~Redis()
    {
        DisConnect();
    }
    Redis& operator=(const Redis& );

    bool is_connect()const
    {
        return !(m_ctxt == NULL);
    }
    int errcode()const
    {
        return m_errcode;
    }
    const std::string& errmsg()const
    {
        return m_errmsg;
    }
    const RedisInfo& info()const
    {
        return m_info;
    }
    void swap(Redis& h);
    bool Ping(void);

    void Reset(const std::string& host,
               const int port,
               const struct timeval& time_out);
    void Reset(const RedisInfo& _info)
    {
        Reset(_info.host, _info.port, _info.time_out);
    }
    void Reset(const Redis& h)
    {
        Reset(h.m_info);
    }

    int Connect(void);
    void DisConnect(void);
    int Update(void)
    {
        DisConnect();
        return Connect();
    }

    redisReply* Command(const char* format, ...);
    redisReply* CommandArgv(const std::vector<std::string>& argvec);
    int AppendCommand(const char* format, ...);
    redisReply* GetReply(void);
    int GetAllReply(size_t num, std::vector<redisReply*>&);

    redisReply* vCommand(const char* format, va_list argptr);
    int AppendCommandArgv(const std::vector<std::string>& argvec);
    std::vector<redisReply*> AppendArgvs(const std::vector<
                                         std::vector<std::string> >& argvs);

    redisReply* TryCommand(const char* format, ...);
    redisReply* vTryCommand(const char* format, va_list argptr);
    redisReply* TryCommandArgv(const std::vector<std::string>& argvec);
    std::vector<redisReply*> TryAppendArgvs(const std::vector<
                                            std::vector<std::string> >& argvs);

//extern:
    bool Expire(const std::string& key, int expire_time);

private:
    int m_errcode;
    std::string m_errmsg;
    redisContext *m_ctxt;
    RedisInfo m_info;
};

inline bool Redis::Ping(void)
{
    redisReply* r = Command("PING");
    bool ret = true;
    if (!r || !r->str || strcmp(r->str, "PONG")) ret = false;
    if (r) freeReplyObject(r);
    return ret;
}

inline bool Redis::Expire(const std::string& key, int expire_time)
{
    redisReply* r = Command("EXPIRE %s %d", key.c_str(), expire_time);
    if (!r) return false;
    if (r->type == REDIS_REPLY_ERROR)
    {
        m_errcode = r->integer;
        m_errmsg = "reply:" + std::string(r->str);
        return false;
    }
    return true;
}

}
}
#endif
