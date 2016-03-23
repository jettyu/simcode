#ifndef SIMCODE_LOG_LOGASYNC_H
#define SIMCODE_LOG_LOGASYNC_H
#include <simcode/log/logfile.h>
#include <simcode/base/typedef.h>
#include <simcode/base/logger.h>
#include <simcode/base/noncopyable.h>
#include <simcode/base/OutBuffer.h>
namespace simcode
{
namespace log
{

class LogAsync : noncopyable, public BaseLogger
{
public:
    LogAsync();
    virtual ~LogAsync();
    int Init(const std::string& filename, uint64_t rotate_size);
    void log_out(int level,
                 const char* filename,
                 int linenum,
                 const char* funcname,
                 const char* msg);
private:
    void Write();
private:
    OutBuffer buffer_;
    LogFile logFile_;
    simex::shared_ptr<simex::thread> thread_;
    Mutex mutex_;
    simex::condition_variable cond_;
    bool isStarted_;
};

class GlobalLogAsync
{
public:
    static int Init(const std::string& name, uint64_t rotate_size, int level);
};

}
}
#endif // SIMCODE_LOG_LOGASYNC_H
