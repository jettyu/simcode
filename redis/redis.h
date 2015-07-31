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
    RedisReply():reply_(NULL) {}
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
        Free();
        reply_ = r;
        return *this;
    }
    operator const bool () const
    {
        return reply_!=NULL;
    }
    void reset(redisReply* r)
    {
        Free();
        reply_ = r;
    }
    void Free(void)
    {
        if (reply_) freeReplyObject(reply_);
        reply_ = NULL;
    }

private:
    RedisReply( const RedisReply& ) {}
    const RedisReply& operator=( const RedisReply& ) {}
private:
    redisReply* reply_;
};

class RedisReplyObj
{
public:
    RedisReplyObj(redisReply* r=NULL):reply_(r) {}

    redisReply* operator->() const
    {
        return reply_;
    }

    operator const bool () const
    {
        return reply_!=NULL;
    }

private:
    redisReply* reply_;
};

class RedisReplyList
{
public:
    RedisReplyList(const std::vector<redisReply*>& rs):
        redisReplys_(rs)
    {
        it_ = redisReplys_.begin();
    }
    ~RedisReplyList()
    {
        Free();
    }
    RedisReplyObj FetchReply()
    {
        if (it_==redisReplys_.end()) return RedisReplyObj(NULL);
        return RedisReplyObj(*(++it_-1));
    }
    void Free()
    {
        for (it_=redisReplys_.begin(); it_!=redisReplys_.end(); ++it_)
            freeReplyObject(*it_);
        redisReplys_.clear();
    }
    bool isFetchAll() const
    {
        return it_ == redisReplys_.end();
    }
    void resetFetch()
    {
        it_ = redisReplys_.begin();
    }
    operator const bool () const
    {
        return !redisReplys_.empty();
    }
    size_t size() const
    {
        return redisReplys_.size();
    }
private:
    RedisReplyList( const RedisReplyList& ) {}
    const RedisReplyList& operator=( const RedisReplyList& )
    {
        return *this;
    }
private:
    std::vector<redisReply*> redisReplys_;
    std::vector<redisReply*>::iterator it_;
};

class Redis
{
public:
    Redis():errcode_(1),ctx_(NULL) {}
    Redis(const RedisInfo& _info):errcode_(1),ctx_(NULL)
    {
        Reset(_info);
    }
    Redis(const std::string& host,
          const int port,
          const struct timeval& time_out)
        :errcode_(1), ctx_(NULL)
    {
        Reset(host, port, time_out);
    }
    ~Redis()
    {
        DisConnect();
    }

    bool is_connect()const
    {
        return !(ctx_ == NULL);
    }
    int errcode()const
    {
        return errcode_;
    }
    const std::string& errmsg()const
    {
        return errmsg_;
    }
    const RedisInfo& info()const
    {
        return info_;
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
        Reset(h.info_);
    }

    int Connect(void);
    void DisConnect(void);
    int Update(void)
    {
        DisConnect();
        return Connect();
    }

    redisReply* vCommand(const char* format, va_list argptr);
    redisReply* Command(const char* format, ...);
    redisReply* CommandArgvPrev(int argc, const char **argv, const size_t *argvlen);
    redisReply* CommandArgv(const std::vector<std::string>& argvec);

    int AppendCommand(const char* format, ...);
    redisReply* GetReply(void);
    int GetAllReply(size_t num, std::vector<redisReply*>&);


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
    Redis( const Redis& ) {}
    const Redis& operator=( const Redis& )
    {
        return *this;
    }

private:
    int errcode_;
    std::string errmsg_;
    redisContext *ctx_;
    RedisInfo info_;
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
        errcode_ = r->integer;
        errmsg_ = "reply:" + std::string(r->str);
        return false;
    }
    return true;
}

}
}
#endif
