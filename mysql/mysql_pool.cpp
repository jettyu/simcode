#include <simcode/mysql/mysql_pool.h>
#include <simcode/net/EventLoop.h>

using namespace simcode;
using namespace mysql;

MysqlPool::MysqlPool(simcode::net::EventLoop* loop,
    const simex::function<SharedPtr<Mysql>()>& f)
    :
    default_max_(0),
    active_max_(0),
    life_time_max_(20),
    active_size_(0),
    pool_size_(0),
    loop_(loop),
    new_object_callback_(f),
    timer_state_(0),
    is_busy_(false),
    is_closed_(false)
{
    timerfd_ = loop->runEvery(1.0, simex::bind(&MysqlPool::timerHandle, this));
}

MysqlPool::~MysqlPool()
{
    Close();
}

SharedPtr<Mysql> MysqlPool::Get()
{
    SharedPtr<Mysql> p;
    if (is_closed_) return p;
    is_busy_ = true;
    p = getFromPool();
    if (!p) p = newObject();
    return p;
}

void MysqlPool::Put(const SharedPtr<Mysql>& p)
{
    if (is_closed_) return;
    ScopeLock lock(pool_mtx_);
    pool_.push_back(p);
    pool_size_++;
}

void MysqlPool::Init()
{
    for (int i=0; i<default_max_; i++) 
    {
        SharedPtr<Mysql> p = newObject();
        Put(p);
    }
}

void MysqlPool::Close()
{
    if (is_closed_) return;
    is_closed_ = true;
    loop_->cancelTimer(timerfd_);
    ScopeLock lock(pool_mtx_);
    pool_.clear();
}

SharedPtr<Mysql> MysqlPool::getFromPool()
{
    SharedPtr<Mysql> p;
    ScopeLock lock(pool_mtx_);
    if (!pool_.empty())
    {
        p = pool_.front();
        pool_.pop_front();
        pool_size_--;
    }
    return p;
}

SharedPtr<Mysql> MysqlPool::newObject()
{
    SharedPtr<Mysql> p;
    if (active_size_ < active_max_)
    {
        p = new_object_callback_();
        if (p) active_size_++;
    }
    return p;
}

void MysqlPool::timerHandle()
{
    if (is_closed_) return;
    if (pool_size_ > default_max_)
    {
        if (++timer_state_ > life_time_max_ || !is_busy_)
        {
            ScopeLock lock(pool_mtx_);
            pool_.pop_front();
            pool_size_--;
            active_size_--;
        }
    }
    else
    {
        timer_state_ = 0;
    }
    if (!is_busy_)
    {
        SharedPtr<Mysql> p = getFromPool();
        if (p)
        {
            p->Ping();
            Put(p);
        }
    }
    is_busy_ = false;
}

