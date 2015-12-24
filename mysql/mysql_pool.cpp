#include <simcode/mysql/mysql_pool.h>
#include <simcode/net/EventLoop.h>

using namespace simcode;
using namespace mysql;

MysqlPool::MysqlPool(simcode::net::EventLoop* loop,
    const simex::function<SharedPtr<Mysql>()>& f)
    :
    default_max_(0),
    active_max_(0),
    active_size_(0),
    loop_(loop),
    new_object_callback_(f)
{
    loop->runEvery(1.0, simex::bind(&MysqlPool::timerHandle, this));
}

MysqlPool::~MysqlPool()
{
}

SharedPtr<Mysql> MysqlPool::Get()
{
    SharedPtr<Mysql> p = getDefault();
    if (p) return p;
    p = getActive();
    if (!p)
    {
    }
    return p;
}
void MysqlPool::Put(const SharedPtr<Mysql>& p)
{
    if (0 != putDefault(p))
    {
        if (0 != putActive(p))
        {
            active_size_--;
        }
    }
}

SharedPtr<Mysql> MysqlPool::getDefault()
{
    SharedPtr<Mysql> p;
    ScopeLock lock(default_mtx_);
    if (!default_deque_.empty())
    {
        p = default_deque_.front();
        default_deque_.pop_front();
    }
    return p;
}
int MysqlPool::putDefault(const SharedPtr<Mysql>& p)
{
    ScopeLock lock(default_mtx_);
    if (default_deque_.size() < default_max_)
    {
        default_deque_.push_back(p);
        return 0;
    }
    return 1;
}

SharedPtr<Mysql> MysqlPool::getActive()
{
    SharedPtr<Mysql> p;
    ScopeLock lock(active_mtx_);
    if (!active_deque_.empty())
    {
        p = active_deque_.front();
        active_deque_.pop_front();
    } else if (active_size_ < active_max_)
    {
        active_size_++;
        p = newObject();
    } 
    return p;
}
int MysqlPool::putActive(const SharedPtr<Mysql>& p)
{
    ScopeLock lock(active_mtx_);
    if (active_deque_.size() < active_max_)
    {
        active_deque_.push_back(p);
        return 0;
    }
    return 1;
}
SharedPtr<Mysql> MysqlPool::newObject()
{
    return new_object_callback_();
}

void MysqlPool::timerHandle()
{
    SharedPtr<Mysql> p = getDefault();
    if (p)
    { 
        p->Ping(); 
        Put(p);
    }
    p = onlyGetActive();
    if (p) 
    {
        const simex::any& c = p->getContext();
        int i = 0;
        if (c.empty()) {p->setContext(i);}
        i = simex::any_cast<int>(p->getContext());
        if (i > 20) 
        {
            active_size_--;
            return; //超过20秒回收
        }
        p->setContext(++i);
        p->Ping();
        Put(p);
    }
}

SharedPtr<Mysql> MysqlPool::onlyGetActive()
{
    SharedPtr<Mysql> p;
    ScopeLock lock(active_mtx_);
    if (!active_deque_.empty())
    {
        p = active_deque_.front();
        active_deque_.pop_front();
    }
    return p;
}

