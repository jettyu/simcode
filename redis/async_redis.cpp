#include <simcode/redis/async_redis.h>

using namespace simcode;
using namespace redis;

AsyncRedis::AsyncRedis() :
    ctx_(NULL)
{

}
AsyncRedis::~AsyncRedis()
{
}
int AsyncRedis::Connect()
{
    redisAsyncContext *c = redisAsyncConnect(info_.host.c_str(), info_.port);
    if (c->err)
    {
        /* Let *c leak for now... */
        printf("Error: %s\n", c->errstr);
        return 1;
    }
    else
    {
        ctx_ = c;
        if (connectCallback_) connectCallback_(ctx_);
        redisAsyncSetConnectCallback(ctx_, connectCallback);
        redisAsyncSetDisconnectCallback(ctx_, disconnectCallback);
    }
}

int AsyncRedis::vCommand(const CommandCallback& b, const char* format, va_list argptr)
{
    if (!isConnected()) return REDIS_DISCONNECTING;
    CallbackData* data = new CallbackData(this, b);
    return redisvAsyncCommand(ctx_, commandCallback, data, format, argptr);
}

int AsyncRedis::Command(const CommandCallback& b, const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    int ret = vCommand(b, format, argptr);
    va_end(argptr);
    return ret;
}

int AsyncRedis::CommandArgv(const CommandCallback& b, const std::vector<std::string>& argvec)
{
    if (!isConnected()) return REDIS_DISCONNECTING;
    int argc = argvec.size();
    std::vector<const char *> argv;
    std::vector<size_t> argvlen;
    argv.reserve(argc);
    argvlen.reserve(argc);
    std::vector<std::string>::const_iterator it;
    for (it=argvec.begin(); it!=argvec.end(); ++it)
    {
        argv.push_back(it->c_str());
        argvlen.push_back(it->length());
    }
    CallbackData* data = new CallbackData(this, b);
    return redisAsyncCommandArgv(ctx_, commandCallback, data,
                                          argc, argv.data(), argvlen.data());
}

/*static*/
void AsyncRedis::connectCallback(const struct redisAsyncContext* c, int status)
{
    if (status == REDIS_OK)
    {
        printf("redis connect success!\n");
    }
    else
    {
        printf("redis connect failed\n");
        if (c != NULL)
        {
            printf("errmsg=%s\n", c->errstr);
        }
    }
}

/*static*/
void AsyncRedis::disconnectCallback(const redisAsyncContext *c, int status)
{
    printf("redis disconnected!\n");
}

/*static*/
void AsyncRedis::commandCallback(redisAsyncContext* ac, void* r, void* privdata)
{
    redisReply* reply = static_cast<redisReply*>(r);
    CallbackData* data = static_cast<CallbackData*>(privdata);
    if (data->cb) data->cb(data->async_redis, reply);
    delete data;
}
