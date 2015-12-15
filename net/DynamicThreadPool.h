#ifndef __SIMCODE_NET_DYNAMIC_THREAD_POOL_H__
#define __SIMCODE_NET_DYNAMIC_THREAD_POOL_H__
#include <simcode/net/EventLoop.h>
#include <simcode/base/typedef.h>
namespace simcode {
namespace net {
class ThreadInfo
{
public:
    ThreadInfo():status(0){}
    ~ThreadInfo(){}
    std::atomic<int> status; //100表示stop
    SimThreadPtr thread_ptr;
    void stop()
    {
        status.store(100); 
        if (thread_ptr) thread_ptr->join();
    }
};
class DynamicThreadPool
{
public:
    typedef simex::function<void(simex::any* context)> TaskCallback;
    DynamicThreadPool(EventLoop*);
    virtual ~DynamicThreadPool();
    void start();
    void stop();
    int addTask(const TaskCallback& cb);
    void setMaxIdle(int n){maxIdle_ = n;}
    void setMaxActive(int n){maxActive_ = n;}
    void setMaxTaskSize(int n){maxTaskSize_ = n;}
private:
    void AddThread();
    void DelThread(const SharedPtr<ThreadInfo>&);
    void addThread();
    void timerHandle();
    void doTask(const SharedPtr<ThreadInfo>&);
private:
    
    EventLoop* loop_; //定时器
    std::vector<SharedPtr<ThreadInfo>> defaultPool_;
    std::map<std::thread::id, SharedPtr<ThreadInfo>> pool_;
    std::deque<TaskCallback> deq_;
    Mutex mtx_;
    ConditionVar cond_;
    int maxIdle_;
    int maxActive_;
    int maxTaskSize_;
    std::atomic<int> threadNum_;
    std::atomic<int> isClosed_;
};

}//endof namespace net
}//endof namespace simcode

#endif
