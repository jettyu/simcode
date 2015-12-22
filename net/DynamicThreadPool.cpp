#include <simcode/net/DynamicThreadPool.h>

using namespace simcode;
using namespace net;

DynamicThreadPool::DynamicThreadPool(EventLoop* loop)
    :
     loop_(loop),
     maxIdle_(1),
     maxActive_(0),
     maxTaskSize_(1024),
     threadNum_(0),
     isClosed_(0)
{
}
DynamicThreadPool::~DynamicThreadPool()
{
    stop();
}

void DynamicThreadPool::start()
{
    loop_->runEvery(1.0, simex::bind(&DynamicThreadPool::timerHandle, this));
    defaultPool_.reserve(maxIdle_);
    for (size_t i=0; i<maxIdle_; ++i)
    {
        SharedPtr<ThreadInfo> t(new ThreadInfo);
        t->thread_ptr.reset(new SimThread(SimBind(&DynamicThreadPool::doTask, this, t)));
        defaultPool_.push_back(t);
    }
}

void DynamicThreadPool::stop()
{
    if (isClosed_ == 1) return;
    isClosed_ = 1;
    cond_.notify_all();
    std::vector<SharedPtr<ThreadInfo>>::iterator vit;
    for (vit=defaultPool_.begin(); vit!=defaultPool_.end(); ++vit)
    {
        (*vit)->stop();
    }
    {
        std::map<std::thread::id, SharedPtr<ThreadInfo>>::iterator mit;
        ScopeLock lock(mapMtx_);
        for (mit=pool_.begin(); mit!=pool_.end(); ++mit)
        {
            mit->second->stop();
        }
    }
}

int DynamicThreadPool::taskNum()
{
    ScopeLock lock(mtx_);
    return deq_.size();
}

void DynamicThreadPool::busyThread(std::vector<std::thread::id>& vec)
{
    std::vector<SharedPtr<ThreadInfo>>::iterator it;
    for (it=defaultPool_.begin(); it!=defaultPool_.end(); ++it)
    {
        if ((*it)->is_busy)
            vec.push_back((*it)->thread_ptr->get_id());
    }
    {
        std::map<std::thread::id, SharedPtr<ThreadInfo>>::iterator mit;
        ScopeLock lock(mapMtx_);
        for (mit=pool_.begin(); mit!=pool_.end(); ++mit)
        {
            if (mit->second->is_busy)
                vec.push_back(mit->second->thread_ptr->get_id());
        }
    }
}

int DynamicThreadPool::addTask(const TaskCallback& cb)
{
    if (isClosed_) return 1;
    ScopeLock lock(mtx_);    
    if (deq_.size() < maxTaskSize_)
    {
        if (deq_.size() > threadNum_*5 && threadNum_.load() < maxTaskSize_)
        {
            turnOn();
            addThread();
        }
        else
        {
            turnOff();
        }
        deq_.push_back(cb);
        cond_.notify_one();
    }
    else
    {
        return 1;
    }
    return 0;
}

void DynamicThreadPool::AddThread()
{
    SharedPtr<ThreadInfo> t(new ThreadInfo);
    threadNum_++;
    t->thread_ptr.reset(new SimThread(SimBind(&DynamicThreadPool::doTask, this, t)));
    t->is_dynamic = true;
    {
    ScopeLock lock(mapMtx_);
    pool_[t->thread_ptr->get_id()] = t;
    }
}

void DynamicThreadPool::DelThread(const SharedPtr<ThreadInfo>& t)
{
    ScopeLock lock(mapMtx_);
    pool_.erase(t->thread_ptr->get_id());
}

void DynamicThreadPool::addThread()
{
    loop_->addTask(simex::bind(&DynamicThreadPool::AddThread, this));
}

void DynamicThreadPool::timerHandle()
{
    if (isClosed_) return;
    ScopeLock lock(mapMtx_);
    std::map<std::thread::id, SharedPtr<ThreadInfo>>::iterator it;
    for (it=pool_.begin(); it!=pool_.end(); ++it)
    {
        it->second->status++;
    }
}

void DynamicThreadPool::doTask(const SharedPtr<ThreadInfo>& ti)
{
    {
    bool flag = true;
    ScopeLock lock(mtx_);
    while (ti->status < 0x3 && !isClosed_)
    {
        while(!deq_.empty() && flag && (isTurnOn() || !ti->is_dynamic))
        {
            TaskCallback e = deq_.front();
            deq_.pop_front();
            ti->status = 0;
            lock.unlock();
            ti->is_busy = true;
            e();
            ti->is_busy = false;
            lock.lock();
        }
        if (cond_.wait_for(lock,std::chrono::seconds(3)) == std::cv_status::timeout)
        {
            flag = false;
        }
        else
        {
            flag = true;
        }
    }
    }
    threadNum_--;
    if (ti->is_dynamic && !isClosed_)
    {
        loop_->addTask(simex::bind(&DynamicThreadPool::DelThread, this, ti));
    }
}
