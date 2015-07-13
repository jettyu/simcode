#ifndef SIMCODE_ASYNC_REDIS_H
#define SIMCODE_ASYNC_REDIS_H

#include<simcode/base/typedef.h>
#include <simcode/redis/redis.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <vector>
#include <string>
namespace simcode
{
namespace redis
{
class AsyncRedis
{
public:
    typedef simex::function<void(AsyncRedis*, redisReply*)> CommandCallback;
    typedef simex::function<void(redisAsyncContext* c)> AttachCallback;
    typedef simex::function<void(AsyncRedis* ar, int status)> ConnectCallback;
    typedef simex::function<void(AsyncRedis* ar, int status)> DisconnectCallback;
    AsyncRedis();
    ~AsyncRedis();
    void set_redisinfo(const RedisInfo& info__)
    {
        info_ = info__;
    }
    const RedisInfo& info()const
    {
        return info_;
    }
    redisAsyncContext* getContext()
    {
        return ctx_;
    }
    const bool isConnected() const
    {
        return ctx_ != NULL;//&& (ctx_->c.flags & REDIS_CONNECTED);
    }
    void set_attachCallback(const AttachCallback& b)
    {
        attachCallback_ = b;
    }
    int Connect();
    void set_connectCallback( const ConnectCallback& fn)
    {
        connectCallback_ = fn;
    }
    void set_disconnectCallback(const DisconnectCallback& fn)
    {
        disconnectCallback_ = fn;
    }
    void set_retry(bool retry__)
    {
        retry_ = retry__;
    }
    int vCommand(const CommandCallback& b, const char* format, va_list argptr);
    int Command(const CommandCallback& b, const char* format, ...);
    int CommandArgvPrev(const CommandCallback& b, int argc, const char **argv, const size_t *argvlen);
    int CommandArgv(const CommandCallback& b, const std::vector<std::string>& argvec);

private:
    static void connectCallback(const redisAsyncContext* c, int status);
    static void disconnectCallback(const redisAsyncContext *c, int status);
    static void commandCallback(redisAsyncContext* c, void* r, void* privdata);
private:
    struct CallbackData {
        CallbackData(AsyncRedis* r, const CommandCallback& b):
            async_redis(r), cb(b)
        {
        }
        AsyncRedis* async_redis;
        CommandCallback cb;
    };
private:
    RedisInfo info_;
    redisAsyncContext* ctx_;
    AttachCallback attachCallback_;
    ConnectCallback connectCallback_;
    DisconnectCallback disconnectCallback_;
    bool retry_;
    int retryTime_;
};

}
}

#endif // SIMCODE_ASYNC_REDIS_H
