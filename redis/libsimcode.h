#ifndef SIMCODE_REDIS_LIB_SIMCODE_H
#define SIMCODE_REDIS_LIB_SIMCODE_H
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <simcode/net/EventChannel.h>

namespace simcode
{
namespace redis
{

typedef struct redisLibSimcode
{
    redisAsyncContext* context;
    simcode::net::EventChannel* channel;
} redisLibSimcode;

static void addRead(void *privdata)
{
    redisLibSimcode *e = (redisLibSimcode*)privdata;
    e->channel->enableReading();
}

static void delRead(void *privdata)
{
    redisLibSimcode *e = (redisLibSimcode*)privdata;
    e->channel->disableReading();
}

static void addWrite(void *privdata)
{
    redisLibSimcode *e = (redisLibSimcode*)privdata;
    e->channel->enableWriting();
}

static void delWrite(void *privdata)
{
    redisLibSimcode *e = (redisLibSimcode*)privdata;
    e->channel->disableWriting();
}

static void cleanup(void *privdata)
{
    redisLibSimcode *e = (redisLibSimcode*)privdata;
    e->channel->getLoop()->removeInLoop(e->channel->fd());
    //delete e->channel;
    free(e);
}

static void eventCallback(simcode::net::EventChannel* channel, redisAsyncContext* ac)
{
    if (channel->isReading()) redisAsyncHandleRead(ac);
    else if (channel->isWriting()) redisAsyncHandleWrite(ac);
}

static int redisLibSimcodeAttach(redisAsyncContext* ac, simcode::net::EventLoop* loop)
{
    redisLibSimcode* e;
    redisContext* c = &ac->c;
    if (ac->ev.data != NULL)
        return REDIS_ERR;
    e = (redisLibSimcode*)malloc(sizeof(*e));
    e->context = ac;
    e->channel = new simcode::net::EventChannel(loop, c->fd, simex::bind(eventCallback, _1, ac));
    simcode::net::EventChannelPtr ec(e->channel);
    loop->runInLoop(ec);

    ac->ev.addRead = addRead;
    ac->ev.delRead = delRead;
    ac->ev.addWrite = addWrite;
    ac->ev.delWrite = delWrite;
    ac->ev.cleanup = cleanup;
    ac->ev.data = e;
    return REDIS_OK;
}

}
}
#endif
