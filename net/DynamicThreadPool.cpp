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
        t->thread_ptr->detach();
        defaultPool_.push_back(t);
    }
}

void DynamicThreadPool::stop()
{
    if (isClosed_ == 1) return;
    isClosed_ = 1;
    cond_.notify_all();
    std::for_each(defaultPool_.begin(), defaultPool_.end(),
                  simex::bind(&ThreadInfo::stop, _1));
}

int DynamicThreadPool::addTask(const TaskCallback& cb)
{
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
    t->status = 0x1;
	t->is_dynamic = true;
    pool_[t->thread_ptr->get_id()] = t;
}

void DynamicThreadPool::DelThread(const SharedPtr<ThreadInfo>& t)
{
    pool_.erase(t->thread_ptr->get_id());
}

void DynamicThreadPool::addThread()
{
    loop_->addTask(simex::bind(&DynamicThreadPool::AddThread, this));
}

void DynamicThreadPool::timerHandle()
{
    std::map<std::thread::id, SharedPtr<ThreadInfo>>::iterator it;
    for (it=pool_.begin(); it!=pool_.end(); ++it)
    {
        it->second->status++;
        if (it->second->status > 3 || isClosed_ != 0) 
        {
            it->second->status = 0xf;
        }
    }
}

void DynamicThreadPool::doTask(const SharedPtr<ThreadInfo>& ti)
{
    bool flag = true;
    simex::any context;
    ScopeLock lock(mtx_);
    while (ti->status < 0xf && isClosed_ == 0)
    {
        while(!deq_.empty() && flag && (isTurnOn() || !ti->is_dynamic))
        {
            TaskCallback e = deq_.front();
            deq_.pop_front();
            ti->status = 0;
            lock.unlock();
            e();
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
//        cond_.wait(lock);
    }
    threadNum_--;
    loop_->addTask(simex::bind(&DynamicThreadPool::DelThread, this, ti));
}
