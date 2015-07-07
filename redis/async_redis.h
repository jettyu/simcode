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
    typedef simex::function<void(redisAsyncContext* c)> ConnectCallback;
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
    void set_connectCallback(const ConnectCallback& b)
    {
        connectCallback_ = b;
    }

    const bool isConnected() const
    {
        return ctx_ != NULL;//&& (ctx_->c.flags & REDIS_CONNECTED);
    }
    redisAsyncContext* getContext()
    {
        return ctx_;
    }
    int Connect();
    int vCommand(const CommandCallback& b, const char* format, va_list argptr);
    int Command(const CommandCallback& b, const char* format, ...);
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
    ConnectCallback connectCallback_;
};

}
}

#endif // SIMCODE_ASYNC_REDIS_H
