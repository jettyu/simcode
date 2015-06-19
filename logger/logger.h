#ifndef SIMCODE_LOGGER_LOGGER_H
#define SIMCODE_LOGGER_LOGGER_H
#include <simcode/logger/logfile.h>
#include <simcode/base/typedef.h>
#include <simcode/base/logger.h>
namespace simcode
{
namespace logger
{
class Logging : public simcode::BaseLogger
{
public:
    int Init(const std::string& filename, int level, uint64_t rotate_size);
    void log_out(int level,
                 const char* filename,
                 int linenum,
                 const char* funcname,
                 const char* msg);
private:
    simcode::Logger logger_;
    LogFile logFile_;
    Mutex mutex_;
};

}
}

#endif // SIMCODE_LOGGER_LOGGER_H
