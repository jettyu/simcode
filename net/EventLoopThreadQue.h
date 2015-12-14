#ifndef __SIMCODE_NET_EVENTLOOP_THREAD_QUE_H__
#define __SIMCODE_NET_EVENTLOOP_THREAD_QUE_H__
#include <simcode/net/EventLoopThread.h>
#include <simcode/thread/thread_queue_ex.h>
namespace simcode {
namespace net {

class EventLoopThreadQue
{
public:
    typedef simex::function<void(simex::any *context)> TaskCallback;
    EventLoopThreadQue(int maxIdle, int maxActive, int defaultIdle=1, int defaultSize=10240):
        task_que_(maxActive)
    {
        thread_que_.reset(new simcode::thread::ThreadQueueEx);
        thread_que_->set_threadcount(maxIdle);
        thread_que_->set_maxsize(256);
        default_que_.reset(new simcode::thread::ThreadQueueEx);
        default_que_->set_threadcount(defaultIdle);
        default_que_->set_maxsize(defaultSize);
    }
    virtual ~EventLoopThreadQue()
    {
        stop();
    }
    void start()
    {
        thread_que_->start();
        default_que_->start();
    }
    void stop()
    {
        thread_que_->stop();
        default_que_->stop();
    }
    int addTask(const TaskCallback& cb);
private:
    void put(const EventLoopThreadPtr& lt)
    {
        task_que_.Put(lt);
    }
    void callback(const EventLoopThreadPtr& lt, const TaskCallback& cb)
    {
        cb(lt->loop()->getMutableContext());
        task_que_.Put(lt);
    }
private:
    class TaskQue
    {
    public:
        TaskQue(int max_size) : size_(0), max_size_(max_size){}
        virtual ~TaskQue()
        {
        }
	EventLoopThreadPtr Get()
        {
            EventLoopThreadPtr e = get();
            if (!e && size() << max_size_)
            {
		e.reset(new EventLoopThread);
                e->startLoop();
            }
            return e;
        }        
        void Put(const EventLoopThreadPtr& e)
        {
            {
            ScopeLock lock(mtx_);
            elems_.push_back(e);
            }
            size_++;
        }
        const int size() const
        {
            return size_.load();
        }
    private:
	EventLoopThreadPtr get()
        {
            EventLoopThreadPtr e;
            {
            ScopeLock lock(mtx_);
            if (elems_.empty()) return e;
            e = elems_.front();
            elems_.pop_front();
            }
            size_--;
            return e;
        }
    private: 
        std::deque<EventLoopThreadPtr> elems_;
        Mutex mtx_;
        std::atomic<int> size_;
        int max_size_;
    };

private:
    TaskQue task_que_;
    simcode::thread::ThreadQueueExPtr thread_que_;
    simcode::thread::ThreadQueueExPtr default_que_;
};

} //endof namespace net
} //endof namespace simcode
#endif
