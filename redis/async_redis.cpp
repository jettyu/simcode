#include <simcode/redis/async_redis.h>
#include <unistd.h>
using namespace simcode;
using namespace redis;


AsyncRedis::AsyncRedis() :
    ctx_(NULL),
    retry_(false),
    retryTime_(0)
{
}
AsyncRedis::~AsyncRedis()
{
    freeAll();
}
int AsyncRedis::Connect()
{
    freeAll();
    redisAsyncContext *c = redisAsyncConnect(info_.host.c_str(), info_.port);
    if (c->err)
    {
        /* Let *c leak for now... */
        printf("Error: %s\n", c->errstr);
        return 1;
    }
    else
    {
        c->data = this;
        ctx_ = c;
        if (attachCallback_) attachCallback_(ctx_);
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

int AsyncRedis::CommandArgvPrev(const CommandCallback& b, int argc, const char **argv, const size_t *argvlen)
{
    if (!isConnected()) return REDIS_DISCONNECTING;
    CallbackData* data = new CallbackData(this, b);
    return redisAsyncCommandArgv(ctx_, commandCallback, data,
                                 argc, argv, argvlen);
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

int AsyncRedis::CommandAlway(const CommandCallback& b, const char* format, ...)
{
    if (!isConnected()) return REDIS_DISCONNECTING;
    va_list argptr;
    va_start(argptr, format);
    CallbackData* data = new CallbackData(this, b, true);
    int ret = redisvAsyncCommand(ctx_, commandCallback, data, format, argptr);
    va_end(argptr);
    return ret;
}

int AsyncRedis::CommandArgvAlway(const CommandCallback& b, const std::vector<std::string>& argvec)
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
    CallbackData* data = new CallbackData(this, b, true);
    return redisAsyncCommandArgv(ctx_, commandCallback, data,
                                 argc, argv.data(), argvlen.data());
}

void AsyncRedis::freeAll()
{
    std::list<CallbackData*>::iterator it;
    for (it=alway_data_.begin(); it!=alway_data_.end(); ++it)
    {
        delete *it;
    }
    alway_data_.clear();
}

/*static*/
void AsyncRedis::connectCallback(const struct redisAsyncContext* ac, int status)
{
    if (ac)
    {
        AsyncRedis* ar = static_cast<AsyncRedis*>(ac->data);
        if (ar->connectCallback_)
        {
            ar->connectCallback_(ar, status);
            return;
        }
    }
    if (status == REDIS_OK)
    {
        printf("redis connect success!\n");
    }
    else
    {
        printf("redis connect failed\n");
        if (ac != NULL)
        {
            printf("errmsg=%s\n", ac->errstr);
        }
    }
}

/*static*/
void AsyncRedis::disconnectCallback(const redisAsyncContext *ac, int status)
{
    if (!ac) return;
    AsyncRedis* ar = static_cast<AsyncRedis*>(ac->data);
    if (ar->disconnectCallback_)
    {
        ar->disconnectCallback_(ar, status);
        ar->freeAll();

    }
    else
    {
        printf("redis disconnected!\n");
    }
    if (ar->retry_)
    {
        printf("redis disconnected, now retrying!\n");
        sleep(ar->retryTime_++);
        ar->Connect();
    }
}

/*static*/
void AsyncRedis::commandCallback(redisAsyncContext* ac, void* r, void* privdata)
{
    redisReply* reply = static_cast<redisReply*>(r);
    CallbackData* data = static_cast<CallbackData*>(privdata);
    if (data->cb)
    {
        data->cb(data->async_redis, reply);
    }
    if (!data->is_alway) delete data;
    else data->async_redis->alway_data_.push_back(data);
}
