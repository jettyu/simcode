#include <simcode/net/DynamicThreadPool.h>
#include <simcode/base/logger.h>
#include <sys/time.h>

using namespace simcode;
using namespace net;

DynamicThreadPool::DynamicThreadPool(EventLoop* loop)
    :
     loop_(loop),
     maxIdle_(1),
     maxActive_(0),
     maxTaskSize_(1024),
     maxLifeTime_(10),
     maxWaitTime_(1000),
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
    int timerfd = loop_->runEvery(double(double(maxWaitTime_)/2000), simex::bind(&DynamicThreadPool::timerCreate, this));
    timerfds_.push_back(timerfd);
    timerfd = loop_->runEvery(double(maxLifeTime_)/2, simex::bind(&DynamicThreadPool::timerClose, this));
    timerfds_.push_back(timerfd);
    
    defaultPool_.reserve(maxIdle_);
    for (size_t i=0; i<maxIdle_; ++i)
    {
        threadNum_++;
        SharedPtr<ThreadInfo> t(new ThreadInfo);
        t->thread_ptr.reset(new SimThread(SimBind(&DynamicThreadPool::doTask, this, t)));
        defaultPool_.push_back(t);
    }
}

void DynamicThreadPool::stop()
{
    if (isClosed_) return;
    std::vector<int>::iterator iit;
    for (iit=timerfds_.begin(); iit!=timerfds_.end(); ++iit)
    {
        loop_->cancelTimer(*iit);
    }
    isClosed_ = true;
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
    try
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
    catch(const std::exception& e)
    {
        LOG_ERROR("create thread failed|err=%s", e.what());
    }
    catch(...)
    {
        LOG_ERROR("create thread failed|err=%s", "unknown exception");
    }
    
}

void DynamicThreadPool::DelThread(const SharedPtr<ThreadInfo>& t)
{
    try
    {
        ScopeLock lock(mapMtx_);
        pool_.erase(t->thread_ptr->get_id());
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("remove thread failed|err=%s", e.what());
    }
    catch(...)
    {
        LOG_ERROR("remove thread failed|err=%s", "unknown exception");
    }
}

void DynamicThreadPool::timerCreate()
{
    if (isClosed_) return;
    int64_t lastmsec = curmsec_;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    curmsec_ = int64_t(tv.tv_sec)*1000 
               + int64_t(tv.tv_usec)/1000;
    //检查所有线程状态

    std::vector<SharedPtr<ThreadInfo>>::iterator it;
    for (it=defaultPool_.begin(); it!=defaultPool_.end(); ++it)
    {
        if (!(*it)->is_busy || (lastmsec-(*it)->status) < maxWaitTime_)
        {
            turnOff();
            return;
        }
    }
    turnOn();
    {
        ScopeLock lock(mapMtx_);
        std::map<std::thread::id, SharedPtr<ThreadInfo>>::iterator it;
        for (it=pool_.begin(); it!=pool_.end(); ++it)
        {
            if (!it->second->is_busy 
               || (lastmsec-it->second->status) < maxWaitTime_ )
            {
                return;
            }
        }    
    }
    if (threadNum_.load() < maxActive_)
        AddThread();
}

void DynamicThreadPool::timerClose()
{
    int64_t lastmsec = curmsec_;
    ScopeLock lock(mapMtx_);
    LOG_DEBUG("taskNum=%d|threadNum=%d|maxTaskSize=%d", deq_.size(), threadNum_.load(), maxTaskSize_);
    std::map<std::thread::id, SharedPtr<ThreadInfo>>::iterator it;
    for (it=pool_.begin(); it!=pool_.end(); ++it)
    {
        if ((lastmsec-it->second->status) > maxLifeTime_)
            it->second->is_closed = true;
    }
}

void DynamicThreadPool::doTask(const SharedPtr<ThreadInfo>& ti)
{
    {
    bool flag = true;
    ScopeLock lock(mtx_);
    while (!isClosed_)  
    {
        while(!deq_.empty() && flag)
        {
            TaskCallback e = deq_.front();
            deq_.pop_front();
            lock.unlock();
            ti->status = curmsec_;
            ti->is_busy = true;
            e();
            ti->is_busy = false;
            ti->status = curmsec_;
            lock.lock();
        }
        if (isClosed_ || ti->is_closed || (ti->is_dynamic && !isTurnOn()))
        {
            break;
        } else if (cond_.wait_for(lock,std::chrono::seconds(3)) == std::cv_status::timeout)
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
    if (ti->is_dynamic)
    {
        DelThread(ti);
    }
}
