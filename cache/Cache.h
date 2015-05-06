#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <simcode/base/typedef.h>

namespace simcode
{
namespace cache
{
class Cache
{
public:
    virtual std::string Get(const std::string& key) = 0;
    virtual void Set(const std::string& key, const std::string& value, int64_t expire = 0) = 0;
    virtual void Remove(const std::string& key) = 0;
    virtual int64_t IncrBy(const std::string& key, int inc=1) = 0;

    virtual int errcode() = 0;
    virtual std::string errmsg() = 0;
};

typedef SharedPtr<Cache> CachePtr;
}
}
#endif
