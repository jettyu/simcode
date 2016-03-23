#ifndef SAFEMAP_H
#define SAFEMAP_H

#include <map>
#include <simcode/base/typedef.h>
#include <simcode/base/noncopyable.h>
namespace simcode
{
template<typename Key, typename Val>
class SafeMap : noncopyable
{
public:
    bool get(const Key& k, Val& v);
    bool insert(const Key& k, const Val& v);
    void set(const Key& k, const Val&v);
    void erase(const Key& k);

    bool empty()
    {
        ReadLock rlock(rwmtx_);
        return kvmap_.empty();
    }
    size_t size()
    {
        ReadLock rlock(rwmtx_);
        return kvmap_.size();
    }
private:
    std::map<Key,Val> kvmap_;
    SharedMutex rwmtx_;
};

template<typename Key, typename Val>
bool SafeMap<Key, Val>::get(const Key&k, Val& v)
{
    ReadLock rlock(rwmtx_);
    typename::std::map<Key,Val>::iterator it = kvmap_.find(k);
    if (it != kvmap_.end())
    {
        v = it->second;
        return true;
    }
    return false;
}

template<typename Key, typename Val>
bool SafeMap<Key, Val>::insert(const Key& k, const Val& v)
{
    WriteLock wlock(rwmtx_);
    return kvmap_.insert(std::pair<Key, Val>(k, v)).second;
}

template<typename Key, typename Val>
void SafeMap<Key, Val>::set(const Key& k, const Val& v)
{
    WriteLock wlock(rwmtx_);
    kvmap_[k] = v;
}

template<typename Key, typename Val>
void SafeMap<Key, Val>::erase(const Key& k)
{
    WriteLock wlock(rwmtx_);
    kvmap_.erase(k);
}
}

#endif
