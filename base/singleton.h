#ifndef SINGLETON_H
#define SINGLETON_H

#include <simcode/base/typedef.h>

namespace simcode
{

#define SINGLETON(OBJ) Singleton<OBJ>::instance()

template<class OBJ>
class Singleton
{
public:
    static OBJ* instance();
    OBJ* operator->()
    {
        return instance();
    }
private:
    static void destroy(void);
    static OBJ* obj_;
    static Mutex mtx_;
};

template<class OBJ>
OBJ* Singleton<OBJ>::obj_ = NULL;

template<class OBJ>
Mutex Singleton<OBJ>::mtx_;

template<class OBJ>
OBJ* Singleton<OBJ>::instance()
{
    if (!obj_)
    {
        ScopeLock lock(mtx_);
        if (!obj_)
        {
            obj_ = new OBJ;
            std::atexit(destroy);
        }
    }
    return obj_;
}

template<class OBJ>
void Singleton<OBJ>::destroy(void)
{
    if (obj_) delete obj_;
    obj_ = NULL;
}

}

#endif
