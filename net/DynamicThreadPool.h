#ifndef __SIMCODE_NET_DYNAMIC_THREAD_POOL_H__
#define __SIMCODE_NET_DYNAMIC_THREAD_POOL_H__
#include <simcode/net/EventLoop.h>
#include <simcode/base/typedef.h>
namespace simcode {
namespace net {
class ThreadInfo
{
public:
    ThreadInfo():status(0), is_dynamic(false), is_busy(false), is_closed(false){}
    ~ThreadInfo(){stop();}
    bool is_dynamic; //是否动态创建的
    volatile int status; //0xf表示stop
    volatile bool is_busy; //是否忙碌
    volatile bool is_closed; //是否关闭
    SimThreadPtr thread_ptr;
    void stop()
    {
        is_closed = true;
    }
};
class DynamicThreadPool
{
public:
    typedef simex::function<void()> TaskCallback;
    DynamicThreadPool(EventLoop*);
    virtual ~DynamicThreadPool();
    void start();
    void stop();
    int addTask(const TaskCallback& cb);
    void setMaxIdle(int n){maxIdle_ = n;}
    void setMaxActive(int n){maxActive_ = n;}
    void setMaxTaskSize(int n){maxTaskSize_ = n;}
    void setMaxLifeTime(int n){maxLifeTime_ = n;}
    int taskNum();       //当前队列的任务数
    int threadNum() {return threadNum_.load();}
    void busyThread(std::vector<std::thread::id>&); //当前正在执行任务的线程数
private:
    void AddThread();
    void DelThread(const SharedPtr<ThreadInfo>&);
    void addThread();
    void timerHandle();
    void doTask(const SharedPtr<ThreadInfo>&);
    void turnOn(){dynamic_turn_ = 0xf;}
    void turnOff(){dynamic_turn_ = 0x0;}
    bool isTurnOn(){return dynamic_turn_  == 0xf;}
private:
    
    EventLoop* loop_; //定时器
    std::vector<SharedPtr<ThreadInfo>> defaultPool_;
    std::map<std::thread::id, SharedPtr<ThreadInfo>> pool_;
    std::deque<TaskCallback> deq_;
    Mutex mtx_;
    Mutex mapMtx_;
    ConditionVar cond_;
    int maxIdle_;
    int maxActive_;
    int maxTaskSize_;
    int maxLifeTime_;
    std::atomic<int> threadNum_;
    volatile bool isClosed_;
    volatile bool dynamic_turn_;
};

}//endof namespace net
}//endof namespace simcode

#endif
