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
private:
    SharedPtr<Mysql> getDefault();
    int putDefault(const SharedPtr<Mysql>& p);
    SharedPtr<Mysql> getActive();
    int putActive(const SharedPtr<Mysql>& p);
    SharedPtr<Mysql> newObject();
    void timerHandle();
    SharedPtr<Mysql> onlyGetActive();
    
private:
    std::deque<SharedPtr<Mysql>> default_deque_;
    Mutex default_mtx_;
    int default_max_;
    std::deque<SharedPtr<Mysql>> active_deque_;
    Mutex active_mtx_;
    int active_max_;
    std::atomic<int> active_size_;
    net::EventLoop* loop_;
    simex::function<SharedPtr<Mysql>()> new_object_callback_;
};

}//endof namespace mysql
}//endof namespace simcode

#endif
