#ifndef SIMCODE_NET_TIME_WHEEL_H
#define SIMCODE_NET_TIME_WHEEL_H

#include <simcode/base/typedef.h>
#include <simcode/net/EventLoopThreadPool.h>
#include <boost/circular_buffer.hpp>
#include <boost/unordered_set.hpp>
namespace simcode
{
namespace net
{

class TimerWheel
{
public:
    typedef simex::function<void()> TimerCallback;
    class Node
    {
    public:
        Node(const TimerCallback& c) : timerCallback_(c) {}
        ~Node()
        {
            timerCallback_();
        }
    private:
        TimerCallback timerCallback_;
    };
    typedef SharedPtr<Node> NodePtr;
    typedef simex::weak_ptr<Node> WeakNodePtr;
public:
    TimerWheel(EventLoop* loop, int len, double intervalTime=double(1.0)) :
        loop_(loop), node_buckets_(len), intervalTime_(intervalTime_)
    {
    }
    void start()
    {
        loop_->runEvery(intervalTime_, simex::bind(&TimerWheel::onTimer, this));
    }
    WeakNodePtr AddTimer(int duration, const TimerCallback& c)
    {
        NodePtr ptr(new Node(c));
        ScopeLock lock(mutex_);
        node_buckets_.back().insert(ptr);
        return WeakNodePtr(ptr);
    }
    void Active(const WeakNodePtr& c)
    {
        NodePtr ptr = c.lock();
        if (c) node_buckets_.back().insert(ptr);
    }
private:
    void onTimer()
    {
        ScopeLock lock(mutex_);
        node_buckets_.push_back(Bucket());
    }
private:
    typedef boost::unordered_set<NodePtr> Bucket;
    typedef boost::circular_buffer<Bucket> WeakNodeList;
    EventLoop* loop_;
    WeakNodeList node_buckets_;
    double intervalTime_;
    Mutex mutex_;
};

class TimerWheels
{
public:
    TimerWheels(EventLoopThreadPool* loop_pool__):loop_pool_(loop_pool__)
    {}
    ~TimerWheels()
    {
        ScopeLock lock(mutex_);
        std::map<int, TimerWheel*>::iterator it;
        for (it=wheels_.begin(); it!=wheels_.end(); ++it)
            delete it->second;
        wheels_.clear();
    }
    TimerWheel::WeakNodePtr AddTimer(int duration, const TimerWheel::TimerCallback& c)
    {
        TimerWheel *wheel = NULL;
        std::map<int, TimerWheel*>::iterator it;
        {
            ScopeLock lock(mutex_);
            it = wheels_.find(duration);
            if (it != wheels_.end()) wheel = it->second;
        }


        if (!wheel)
        {
            wheel = new TimerWheel(loop_pool_->getNextLoop(), duration);
            wheel->start();
            {
                ScopeLock lock(mutex_);
                wheels_[duration] = wheel;
            }
        }
        return wheel->AddTimer(duration, c);
    }
    void Active(int duration, const WeakNodePtr& c)
    {
        TimerWheel::NodePtr ptr = c.lock();
        if (ptr)
        {
            std::map<int, TimerWheel*>::iterator it;
            {
                ScopeLock lock(mutex_);
                it = wheels_.find(duration);
                if (it != wheels_.end())
                {
                    it->second.Active(ptr);
                }
            }
        }
    }
private:
    std::map<int, TimerWheel*> wheels_;
    EventLoopThreadPool* loop_pool_;
    Mutex mutex_;
};

}
}

#endif // SIMCODE_NET_TIME_WHELL_H
