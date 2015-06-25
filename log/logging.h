#ifndef SIMCODE_LOGGER_LOGGER_H
#define SIMCODE_LOGGER_LOGGER_H
#include <simcode/log/logfile.h>
#include <simcode/base/typedef.h>
#include <simcode/base/logger.h>
namespace simcode
{
namespace log
{
class Logging : public simcode::BaseLogger
{
public:
    int Init(const std::string& filename, uint64_t rotate_size);
    void log_out(int level,
                 const char* filename,
                 int linenum,
                 const char* funcname,
                 const char* msg);
private:
    LogFile logFile_;
    Mutex mutex_;
};

class GlobalLogging
{
public:
    static int Init(const std::string& name, uint64_t rotate_size, int level);
};

}
}

#endif // SIMCODE_LOGGER_LOGGER_H
