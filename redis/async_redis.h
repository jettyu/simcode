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
    static void set_connectCallback(redisConnectCallback* fn)
    {
        connectCallback_ = fn;
    }
    static void set_disconnectCallback(redisDisconnectCallback* fn)
    {
        disconnectCallback_ = fn;
    }
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
    AttachCallback attachCallback_;
    static redisConnectCallback* connectCallback_;
    static redisDisconnectCallback* disconnectCallback_;
};

}
}

//#include <simcode/redis/async_redis.h>
//#include <hiredis/adapters/libevent.h>
//using namespace simcode;
//using namespace redis;
//using namespace std;
//
//static void Attach(struct event_base* base, redisAsyncContext* c)
//{
//        redisLibeventAttach(c, base);
//}
//
//static void commandCallback(AsyncRedis* ar, redisReply* rp)
//{
//        cout<<"get:"<<rp->str<<endl;
//}
//
//int main()
//{
//        AsyncRedis ar;
//        RedisInfo info;
//        struct event_base *base = event_base_new();
//        info.host = "127.0.0.1";
//        info.port = 6379;
//        info.time_out={1, 1500};
//        ar.set_redisinfo(info);
//        ar.set_attachCallback(simex::bind(Attach, base, _1));
//        ar.Connect();
//        sleep(3);
//        cout<<__LINE__<<endl;
//        cout<<ar.Command(AsyncRedis::CommandCallback(), "SET %s %s", "async", "test")<<endl;
//        ar.Command(simex::bind(commandCallback, _1, _2), "GET %s", "async");
//        event_base_dispatch(base);
//        return 0;
//}

#endif // SIMCODE_ASYNC_REDIS_H
