#include <simcode/redis/async_redis.h>
#include <unistd.h>

#define TEST_LIBEVENT 0 //if test libevent, please change 0 to 1

#if TEST_LIBEVENT
	#include <hiredis/adapters/libevent.h>
#else
	#include <simcode/redis/libsimcode.h>
#endif
using namespace simcode;
using namespace redis;
using namespace std;

static void commandCallback(AsyncRedis* ar, redisReply* rp)
{
        cout<<"get:"<<rp->str<<endl;
}

static void subscribeCallback(AsyncRedis* ar, redisReply* reply, const std::string& key)
{
    if (reply == NULL) return;
    if ( reply->type == REDIS_REPLY_ARRAY && reply->elements == 3 ) {
        if ( strcmp( reply->element[0]->str, "subscribe" ) != 0 ) {
            printf( "Received[%s] channel %s: %s\n",
                    key.c_str(),
                    reply->element[1]->str,
                    reply->element[2]->str );
        }
    }
}

#if TEST_LIBEVENT

static void test_libevent()
{
    AsyncRedis ar;
    RedisInfo info;
    struct event_base *base = event_base_new();
    info.host = "127.0.0.1";
    info.port = 6379;
    info.time_out={1, 1500};
    ar.set_redisinfo(info);
    ar.set_attachCallback(simex::bind(redisLibeventAttach, _1, base));
    ar.Connect();
    sleep(1);
    ar.Command(NULL, "SET %s %s", "async", "test");
    ar.Command(simex::bind(commandCallback, _1, _2), "GET %s", "async");
	std::string subcribe_key = "async_subscribe";
    ar.CommandAlway(simex::bind(subscribeCallback, _1,_2, subcribe_key), "SUBSCRIBE %s", subcribe_key.c_str());
    event_base_dispatch(base);
}

#else

static void test_libsimcode()
{
    AsyncRedis ar;
    RedisInfo info;
    info.host = "127.0.0.1";
    info.port = 6379;
    info.time_out={1, 1500};
    ar.set_redisinfo(info);
    net::EventLoop loop;
    ar.set_attachCallback(simex::bind(redisLibSimcodeAttach, _1, &loop));
    ar.Connect();
    sleep(1);
    ar.Command(NULL, "SET %s %s", "async", "test");
    ar.Command(simex::bind(commandCallback, _1, _2), "GET %s", "async");
	std::string subcribe_key = "async_subscribe";
    ar.CommandAlway(simex::bind(subscribeCallback, _1,_2, subcribe_key), "SUBSCRIBE %s", subcribe_key.c_str());
    loop.loop();
}

#endif

int main()
{
#if TEST_LIBEVENT
    test_libevent();
#else
    test_libsimcode();
#endif
    return 0;
}
