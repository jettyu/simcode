#ifndef SIMCODE_LOGGER_LOGFILE_H
#define SIMCODE_LOGGER_LOGFILE_H
#include <string>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>
#include <simcode/base/typedef.h>

namespace simcode
{
namespace log
{

class LogFile
{
public:
    LogFile();
    ~LogFile();
    enum
    {
        LOG_BUF_LEN	 = 4096
    };
    int open(FILE *fp);
    int open(const std::string& filename,  uint64_t rotate_size=0);
    void close();
    int logWrite(const char*data, int size);//write and flush and rotate
    int logData(const char* data, int size, const char* levelName, char buf[LOG_BUF_LEN]);
    int logWriteOnly(const char* data, int size);//just write
    int logFflush();

private:
    void rotate();
private:
    FILE *fp_;
    std::string filename_;
    uint64_t rotate_size_;

    struct
    {
        uint64_t w_curr;
        uint64_t w_total;
    } stats_;
};

}
}
#endif // SIMCODE_LOG_LOGFILE_H
