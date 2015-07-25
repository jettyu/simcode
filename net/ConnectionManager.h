#ifndef SIMCODE_NET_CONNECTION_MANAGER_H
#define SIMCODE_NET_CONNECTION_MANAGER_H
#include <simcode/net/EventLoop.h>
#include <map>
#include <vector>
namespace simcode
{
namespace net
{
template<typename ID_TYPE, typename CONN_TYPE>
class ConnectionManager : noncopyable
{
public:
    ConnectionManager(EventLoop* loop__) : loop_(loop__)
    {
    }
    ~ConnectionManager() {}
    void Add(const ID_TYPE&id, const CONN_TYPE& conn)
    {
        loop_->addTask(simex::bind(&ConnectionManager::add, this, id, conn));
    }
    void Remove(const ID_TYPE& id)
    {
        loop_->addTask(simex::bind(&ConnectionManager::remove, this, id));
    }
    void SendOne(const ID_TYPE& id, const std::string& data)
    {
        loop_->addTask(simex::bind(&ConnectionManager::sendOne, this, id, data));
    }
    void SendAll(const std::string& data)
    {
        loop_->addTask(simex::bind(&ConnectionManager::sendAll, this, data));
    }
    void SendSome(const std::vector<ID_TYPE>& ids, const std::string& data)
    {
        loop_->addTask(simex::bind(&ConnectionManager::sendSome, this, ids, data));
    }
    //void AddCallback(const simex::function<void(typename ConnectionList*)>& callback)
    //{
    //    loop_->addTask(callback, &this->connectionList_);
    //}
private:
    void add(const ID_TYPE&id, const CONN_TYPE& conn)
    {
        connectionList_[id] = conn;
    }
    void remove(const ID_TYPE& id)
    {
        connectionList_.erase(id);
    }
    void sendOne(const ID_TYPE& id, const std::string& data)
    {
        typename ConnectionList::iterator it;
        it = connectionList_.find(id);
        if (it != connectionList_.end())
            it->second->sendString(data);
    }
    void sendAll(const std::string& data)
    {
        typename ConnectionList::iterator it;
        for (it=connectionList_.begin(); it!=connectionList_.end(); ++it)
            it->second->sendString(data);
    }
    void sendSome(const std::vector<ID_TYPE>& ids, const std::string& data)
    {
        typename std::vector<ID_TYPE>::const_iterator it;
        for (it=ids.begin(); it!=ids.end(); ++it)
            sendOne(*it, data);
    }

private:
    EventLoop* loop_;
    typedef typename std::map<ID_TYPE, CONN_TYPE> ConnectionList;
    ConnectionList connectionList_;
};

}
}

#endif // SIMCODE_NET_CONNECTION_MANAGER_H
