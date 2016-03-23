#ifndef REDIS_CACHE_H
#define REDIS_CACHE_H
#include <simcode/cache/Cache.h>
#include <simcode/redis/redis.h>
#include <simcode/base/typedef.h>
namespace simcode
{
namespace cache
{
typedef SharedPtr<simcode::redis::Redis> RedisPtr;
class RedisCache : public Cache
{
public:
    RedisCache(const RedisPtr& p):redis_(p) {}
    std::string Get(const std::string& key);
    void Set(const std::string& key, const std::string& value, int64_t expire = 0);
    void Remove(const std::string& key);
    int64_t IncrBy(const std::string& key, int inc = 1);
    int errcode()
    {
        return redis_->errcode();
    }
    std::string errmsg()
    {
        return redis_->errmsg();
    }
private:
    RedisPtr redis_;
};

typedef SharedPtr<RedisCache> RedisCachePtr;
}
}
#endif
