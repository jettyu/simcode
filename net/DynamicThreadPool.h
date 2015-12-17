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
    std::atomic<int> status; //0xf表示stop
    bool is_dynamic; //是否动态创建的
    SimThreadPtr thread_ptr;
    void stop()
    {
        status.store(0xf); 
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
    void turnOn(){dynamic_turn_.store(0xf);}
    void turnOff(){dynamic_turn_.store(0x0);}
    bool isTurnOn(){return dynamic_turn_.load() == 0xf;}
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
    std::atomic<int> dynamic_turn_;
};

}//endof namespace net
}//endof namespace simcode

#endif
