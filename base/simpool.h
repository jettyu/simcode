#ifndef SIMPOOL_H
#define SIMPOOL_H
#include <deque>
#include <stdio.h>

namespace simcode
{

template<typename T>
void simswap(T a, T b)
{
    T t=a;
    a=b;
    b=t;
}

template<typename T>class SimPoolHandler;

template<typename T>
class SimPool
{
public:
    SimPool()
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
        return pool_.size();
    }
    bool empty()
    {
        return pool_.empty();
    }
    void clear(void (*free_func)(T)=NULL);
    int push_back(T t);
    int Add(T t)
    {
        return push_back(t);
    }
    void Recover(const T t);

    T get(void);
    SimPoolHandler<T> GetHandler();

private:
    std::deque<T> pool_;
    size_t m_max_size;
};

template<typename T>
class SimPoolHandler
{
public:
    SimPoolHandler():m_pool(NULL), m_member((T)0) {}
    SimPoolHandler(SimPool<T> *pool, T member)
        :m_pool(pool), m_member(member) {}
    ~SimPoolHandler()
    {
        Release();
    }
    T get()
    {
        return m_member;
    }
    T operator-> ()
    {
        return m_member;
    }
    operator const bool ()
    {
        return m_member != (T)0;
    }
    void Destroy(void (*free_func)(T t) = NULL);
    void swap(SimPoolHandler& h);
    SimPoolHandler* ptr(void)
    {
        return this;
    }

private:
    void Release();
    //SimPoolHandler(const SimPoolHandler& _h){}
    SimPoolHandler& operator = (const SimPoolHandler& h)
    {
        return *this;
    }

private:
    SimPool<T> *m_pool;
    T m_member;
};

template<typename T>
inline int SimPool<T>::push_back(T t)
{
    if (pool_.size() >  m_max_size) return 1;
    pool_.push_back(t);
    return 0;
}

template<typename T>
inline T SimPool<T>::get(void)
{
    if (pool_.empty()) return (T)0;
    T t1 = pool_.front();
    pool_.pop_front();
    return t1;
}
template<typename T>
inline void SimPool<T>::clear(void (*free_func)(T t))
{
    if (pool_.empty()) return;
    if (free_func)
    {
        typename std::deque<T>::iterator it;
        for (it=pool_.begin(); it!=pool_.end(); ++it)
            if (*it) free_func(*it);
    }
    pool_.clear();
}

template<typename T>
inline SimPoolHandler<T> SimPool<T>::GetHandler()
{
    if (pool_.empty()) return SimPoolHandler<T>(this, T(0));
    T t1 = pool_.front();
    pool_.pop_front();
    return SimPoolHandler<T>(this, t1);
}

template<typename T>
inline void SimPool<T>::Recover(T t)
{
    if (!t)
    {
        return;
    }
    push_back(t);
}

template<typename T>
inline void SimPoolHandler<T>::Release()
{
    if (m_pool) m_pool->Recover(m_member);
    m_member = (T)0;
    m_pool = NULL;
}

template<typename T>
inline void SimPoolHandler<T>::swap(SimPoolHandler<T>& h)
{
    std::swap(m_pool, h.m_pool);
    std::swap(m_member, h.m_member);
}

template<typename T>
inline void SimPoolHandler<T>::Destroy(void (*free_func)(T t))
{
    m_pool=NULL;
    if (free_func && m_member) free_func(m_member);
    m_member=(T)0;
}

}//end of namespace
#endif
