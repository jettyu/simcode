#include <simcode/cache/redis/RedisCache.h>
#include <simcode/base/command.h>
using namespace std;
using namespace simcode;
using namespace simcode::cache;
using namespace simcode::redis;
std::string RedisCache::Get(const std::string& key)
{
    string value;
    RedisReply r = redis_->TryCommand("GET %s", key.c_str());
    if (r && r->str) value.append(r->str, r->len);
    return value;
}

void RedisCache::Set(const std::string& key,
                     const std::string& value,
                     int64_t expire)
{
    vector<vector<string>> argvs;
    argvs.push_back(command("SET")(key)(value));
    if (expire) argvs.push_back(command("EXPIRE")(key)(expire));
    redis_->TryAppendArgvs(argvs);
}

void RedisCache::Remove(const std::string& key)
{
    string value;
    RedisReply r = redis_->TryCommand("DEL %s", key.c_str());
    r.Free();
}

int64_t RedisCache::IncrBy(const std::string& key, int inc)
{
    RedisReply r = redis_->TryCommandArgv(command("INCRBY")(key)(inc));
    if (r && r->type == REDIS_REPLY_INTEGER)
        return r->integer;
    return 0;
}
