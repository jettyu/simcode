#include <simcode/log/logging.h>
using namespace simcode;
using namespace log;

static const char* LogLevelName[]=
{
    "[TRACE]",
    "[DEBUG]",
    "[INFO]",
    "[WARN]",
    "[ERROR]",
    "[FATAL]"
};

static const char* get_level_name(int level)
{
    return LogLevelName[level-1];
}



int Logging::Init(const std::string& filename, uint64_t rotate_size)
{
    int ret;
    ret = logFile_.open(filename, rotate_size);
    return ret;
}

void Logging::log_out(int level,
                      const char* filename,
                      int linenum,
                      const char* funcname,
                      const char* msg)
{
    char data[LogFile::LOG_BUF_LEN];
    char buf[LogFile::LOG_BUF_LEN];
    int len;
    len = snprintf(data, sizeof(data), "%s|%d|%s|%s",
                   filename, linenum, funcname, msg);
    len = logFile_.logData(data, len,
                           get_level_name(level), buf);
    {
        ScopeLock lock(mutex_);
        int n = logFile_.logWrite(buf, len);
        if (n <= 0)
        {
            printf("logFile.logWrite failed|size=%d|errno=%d|errmsg=%s\n", 
                    len,errno, strerror(errno));
        }
    }
}

int GlobalLogging::Init(const std::string& name, uint64_t rotate_size, int level)
{
    simcode::GlobalLogger::set_level(level);
    Logging* logging = new Logging;
    if (0 != logging->Init(name, rotate_size))
    {
        delete logging;
        return 1;
    }
    simcode::GlobalLogger::set_base_logger(logging);
    return 0;
}


