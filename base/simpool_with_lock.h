#ifndef SIMLOCKPOOL_H
#define SIMLOCKPOOL_H
#include <deque>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <atomic>
#include <typedef.h>

namespace simcode
{

template<typename T>
class SimLockPool
{
public:
    SimLockPool()
    {
        m_max_size = 1024;
    }
    inline void set_max_size(size_t n)
    {
        m_max_size = n;
    };
    size_t max_size() const
    {
        return m_max_size;
    };
    size_t size()
    {
        ScopeLock lock_(m_mtx);
        return pool_.size();
    }
    bool empty()
    {
        ScopeLock lock_(m_mtx);
        return pool_.empty();
    }
    void clear(void (*func)(SimLockPool&)=NULL);
    int push_back(T t);

    T get(void);

private:
    std::deque<T> pool_;
    Mutex m_mtx;
    size_t m_max_size;
};

template<typename T>
inline int SimLockPool<T>::push_back(T t)
{
    ScopeLock lock_(m_mtx);
    if (pool_.size() >  m_max_size) return 1;
    pool_.push_back(t);
    return 0;
}

template<typename T>
inline T SimLockPool<T>::get(void)
{
    ScopeLock lock_(m_mtx);
    if (pool_.empty()) return (T)0;
    T t1 = pool_.front();
    pool_.pop_front();
    return t1;
}
template<typename T>
inline void SimLockPool<T>::clear(void (*func)(SimLockPool<T>&))
{
    ScopeLock lock_(m_mtx);
    if (pool_.empty()) return;
    if (func) func(*this);
    pool_.clear();
}

}//end of namespace sim
#endif
