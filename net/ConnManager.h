#ifndef SIMCODE_NET_CONNECTION_MANAGER_H
#define SIMCODE_NET_CONNECTION_MANAGER_H

namespace simcode
{
namespace net
{
template<typename T>
class BaseConnManager
{
public:
    virtual void add(uint64_t id, const T& v)
    {
    }
    virtual T get(uint64_t id)
    {
        return (T)0;
    }
    virtual void erase(uint64_t id)
    {
    }
};
}
}

#endif
