#ifndef __SIMCODE_MYSQL_POOL_H__
#define __SIMCODE_MYSQL_POOL_H__
#include <simcode/base/typedef.h>
#include <simcode/net/EventLoop.h>
#include <simcode/mysql/mysql.h>
#include <deque>

namespace simcode{
namespace mysql{

class MysqlPool
{
public:
    MysqlPool(net::EventLoop* loop, 
              const simex::function<SharedPtr<Mysql>()>& f);
    virtual ~MysqlPool();
    SharedPtr<Mysql> Get();
    void Put(const SharedPtr<Mysql>& p);
    void setMaxIdle(int n){default_max_=n;}
    void setMaxActive(int n){active_max_=n;}
    void setMaxLifeTime(int n){life_time_max_=n;}
    void Init();
    void Close();
private:
    SharedPtr<Mysql> newObject();
    SharedPtr<Mysql> getFromPool();
    void timerHandle();
    
private:
    std::deque<SharedPtr<Mysql>> pool_;
    Mutex pool_mtx_;
    int default_max_;
    int active_max_;
    int life_time_max_;
    std::atomic<int> active_size_;
    std::atomic<int> pool_size_;
    net::EventLoop* loop_;
    simex::function<SharedPtr<Mysql>()> new_object_callback_;
    int timer_state_;
    volatile bool is_busy_;
    volatile bool is_closed_;
    int timerfd_;
};

}//endof namespace mysql
}//endof namespace simcode

#endif
