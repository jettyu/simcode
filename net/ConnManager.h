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
class BaseConnManager : noncopyable
{
public:
    typedef simex::shared_ptr<BaseConnManager> Ptr;
    virtual ~BaseConnManager(){}
    virtual void Add(const ID_TYPE&id, const CONN_TYPE& conn)
    {
    }
    virtual void Remove(const ID_TYPE& id)
    {
    }
    virtual void SendOne(const ID_TYPE& id, const std::string& data)
    {
    }
    virtual void SendAll(const std::string& data)
    {
    }
    virtual void SendAllBuf(const char* buf, size_t size)
    {
    }
    virtual void SendSome(const std::vector<ID_TYPE>& ids, const std::string& data)
    {
    }
    virtual void AddTask(const simex::function<void (const simex::any& connStore)>& b)
    {
    }
};

template<typename ID_TYPE, typename CONN_TYPE>
class ConnManager : public BaseConnManager<ID_TYPE, CONN_TYPE>
{
public:
    typedef simex::shared_ptr<ConnManager> Ptr;
    ConnManager(EventLoop* loop__, const simex::function<void(const CONN_TYPE&, const std::string&)>& b) :
        loop_(loop__),
        sendCallback_(b)
    {
    }
    virtual ~ConnManager() {}

    void Add(const ID_TYPE&id, const CONN_TYPE& conn)
    {
        loop_->addTask(simex::bind(&ConnManager::add, this, id, conn));
    }
    void Remove(const ID_TYPE& id)
    {
        loop_->addTask(simex::bind(&ConnManager::remove, this, id));
    }
    void SendOne(const ID_TYPE& id, const std::string& data)
    {
        loop_->addTask(simex::bind(&ConnManager::sendOne, this, id, data));
    }
    void SendAll(const std::string& data)
    {
        ScopeLock lock(mutex_);
            broadcastBuf_.append(data.data(), data.size());
            if (broadcastBuf_.size() == data.size())
                loop_->addTask(simex::bind(&ConnManager::sendAll, this));
        //loop_->addTask(simex::bind(&ConnManager::sendAll, this, data));
    }
    void SendAllBuf(const char* buf, size_t size)
    {
        {
            ScopeLock lock(mutex_);
            broadcastBuf_.append(buf, size);
            if (broadcastBuf_.size() == size)
                loop_->addTask(simex::bind(&ConnManager::sendAll, this));
        }
    }
    void SendSome(const std::vector<ID_TYPE>& ids, const std::string& data)
    {
        loop_->addTask(simex::bind(&ConnManager::sendSome, this, ids, data));
    }
    void AddCallback(const simex::function<void (const simex::any& connStore)>& b)
    {
        loop_->addTask(simex::bind(b, &this->connectionList_));
    }
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
            sendCallback_(it->second, data);
    }
    void sendAll()
    {
        std::string buf;
        {
            ScopeLock lock(mutex_);
            if (broadcastBuf_.empty()) return;
            buf.swap(broadcastBuf_);
        }
        typename ConnectionList::iterator it;
        for (it=connectionList_.begin(); it!=connectionList_.end(); ++it)
            sendCallback_(it->second, buf);
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
    std::string broadcastBuf_;
    Mutex mutex_;
    simex::function<void(const CONN_TYPE&, const std::string&)> sendCallback_;
};

}
}

#endif // SIMCODE_NET_CONNECTION_MANAGER_H
