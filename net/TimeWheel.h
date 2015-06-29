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
public:
    TimerWheel(EventLoop* loop, int len) : loop_(loop), node_buckets_(len)
    {
    }
    void start()
    {
        loop->runEvery(double(1.0), simex::bind(&TimerWheel::onTimer, this));
    }
    void AddTimer(int duration, const TimerCallback& c)
    {
        NodePtr ptr(new Node(c));
        ScopeLock lock(mutex_);
        node_buckets_.back().insert(c);
    }
private:
    void onTimer()
    {
        ScopeLock lock(mutex_);
        node_buckets_.push_back(Bucket);
    }
private:
    typedef simex::weak_ptr<Node> WeakNodePtr;
    typedef boost::unordered_set<NodePtr> Bucket;
    typedef boost::circular_buffer<Bucket> WeakNodeList;
    EventLoop* loop_;
    WeakNodeList node_buckets_;
    Mutex mutex_;
};

class TimerWheels
{
public:
    TimerWheels(){}
    ~TimerWheels()
    {
        ScopeLock lock(mutex_);
        std::map<int, TimerWheel*>::iterator it;
        for (it=wheels_.begin(); it!=wheels_.end(); ++it)
            delete it->second;
        wheels_.clear();
    }
    void AddTimer(int duration, const TimerCallback& c)
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
            wheel = new TimerWheel(loop_pool_.getNextLoop(), duration);
            wheel->start();
            {
                ScopeLock lock(mutex_);
                wheels_[duration] = wheel;
            }
        }
        wheel->AddTimer(duration, c);
    }
private:
    std::map<int, TimerWheel*> wheels_;
    EventLoopThreadPool loop_pool_;
    Mutex mutex_;
};

}
}

#endif // SIMCODE_NET_TIME_WHELL_H
