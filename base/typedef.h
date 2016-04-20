#ifndef SIMCODE_TYPEDEF_H
#define SIMCODE_TYPEDEF_H

#include <sstream>

#ifdef BOOST

#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/detail/thread.hpp>
#include <boost/thread/thread_time.hpp>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/any.hpp>
#include <boost/unordered_set.hpp>
#include <boost/circular_buffer.hpp>
inline std::string NewUuid()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::stringstream ss;
    ss << uuid;
    return ss.str();
}

static inline unsigned long long get_local_thread_id()
{

    boost::thread::id tid = boost::this_thread::get_id();
    return *reinterpret_cast<unsigned long long*>(&tid);
}

static boost::thread_specific_ptr<unsigned long long> __local_thread_id__(get_local_thread_id());
static inline unsigned long long cur_thread_id()
{
    return *__local_thread_id__.get();
}
#else

#include <memory>
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <simcode/base/any.h>
#include <unordered_set>
#include <vector>
#include <atomic>
using namespace std::placeholders;
#define get_pointer(shared_pointer) shared_pointer.get()

static inline unsigned long long get_local_thread_id()
{
    std::thread::id tid = std::this_thread::get_id();
    return *reinterpret_cast<unsigned long long *>(&tid);
}

static thread_local unsigned long long __local_thread_id__ = get_local_thread_id();

static inline unsigned long long cur_thread_id()
{
    return __local_thread_id__;
}
#endif // BOOST

namespace simcode
{

namespace simex
{
#ifdef BOOST
using namespace boost;
#define PLACEHOLDERS(_N) _N
#else
using namespace std;
#define PLACEHOLDERS(_N) std::placeholders::_N

template<typename T>
class circular_buffer
{
public:
    circular_buffer(size_t n) : buffer_(n), write_index_(0) {}
    ~circular_buffer()
    {
        std::vector<T>().swap(buffer_);
    }
    void push_back(const T& t)
    {
        buffer_[++write_index_%buffer_.size()] = t;
    }
    T& back()
    {
        return buffer_[write_index_%buffer_.size()];
    }
    void resize(size_t n)
    {
        buffer_.resize(n);
    }
    size_t size() const
    {
        return buffer_.size();
    }
private:
    std::vector<T> buffer_;
    atomic_uint write_index_;
};

#endif // BOOST


} //endof namespace simex

#define SharedPtr simex::shared_ptr
#define WeakPtr simex::weak_ptr
#define SimBind simex::bind
#define SimFunction simex::function
#define SimThread simex::thread


//#define SharedPtr boost::shared_ptr
typedef SharedPtr<SimThread> SimThreadPtr;
typedef simex::mutex Mutex;
typedef simex::unique_lock<Mutex> ScopeLock;
typedef simex::condition_variable ConditionVar;
#ifdef BOOST

typedef simex::shared_mutex            SharedMutex;
typedef simex::unique_lock<SharedMutex>   WriteLock;
typedef simex::shared_lock<SharedMutex>   ReadLock;

#else

typedef std::mutex SharedMutex;
typedef std::lock_guard<Mutex> WriteLock;
typedef std::lock_guard<Mutex> ReadLock;

#endif // BOOST

template<class T>
class SimNoLock
{
public:
    SimNoLock():ix_(0) {}

    virtual void Update()
    {
        int i = ix_ ^ 1;
        ix_ = i;
    }
protected:
    T t_[2];
    int ix_;
};

}

#define simex simcode::simex

#endif
