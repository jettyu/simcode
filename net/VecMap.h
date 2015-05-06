#ifndef VEC_MAP_H
#define VEC_MAP_H
#include <simcode/base/noncopyable.h>
#include <simcode/base/safemap.h>

namespace simcode
{
namespace net
{
template<class T>
class VecMap16 : noncopyable
{
public:
    VecMap16() : objs_(maxVecSize_){}
    T get(uint64_t id)
    {
        T c;
        objs_[id&maxVecSize_].get(id>>offset_, c);
        return c;
    }
    void add(uint64_t id, const T& c)
    {
        objs_[id&maxVecSize_].set(id>>offset_, c);
    }
    void erase(uint64_t id)
    {
        objs_[id&maxVecSize_].erase(id>>offset_);
    }
    
private:
    std::vector<SafeMap<uint64_t, T>> objs_;
    static const int maxVecSize_ = uint16_t(-1);
    static const int offset_ = 16;
};
}
}

#endif
