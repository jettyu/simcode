#ifndef SIMCODE_LOGGER_H
#define SIMCODE_LOGGER_H
#include <stdarg.h>
namespace simcode
{

class LogLevel
{
public:
    static void Init(int debug=2,
                int info=3,
                int warn=4,
                int error=5,
                int fatal=6)
    {
        LEVEL_DEBUG = debug;
        LEVEL_INFO = info;
        LEVEL_WARN = warn;
        LEVEL_ERROR = error;
        LEVEL_FATAL = fatal;
    }
    static int LEVEL_DEBUG;
    static int LEVEL_INFO;
    static int LEVEL_WARN;
    static int LEVEL_ERROR;
    static int LEVEL_FATAL;
};

class Logger
{
public:
    //日志级别，文件名，文件行号，日志内容
    typedef void (*log_func_t)(int level,
                               const char* filename,
                               int linenum,
                               const char* funcname,
                               const char* msg);
    static void set_level(int l);
    static void set_log_fun(log_func_t f);
    static void log_write(int level,
                         const char* filename,
                         int linenum,
                         const char* funcname,
                         const char *fmt, ...);
private:
    static void log_out(int level,
                        const char* filename,
                        int linenum,
                        const char* funcname,
                        const char* msg);
    static void log_out_valist(int level,
                               const char* filename,
                               int linenum,
                               const char* funcname,
                               const char*fmt,
                               va_list ap);
private:
    static int level_;
    static log_func_t log_func_;
};

}

#define LOG_DEBUG(fmt, args...)    \
        simcode::Logger::log_write(simcode::LogLevel::LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##args)
#define LOG_INFO(fmt, args...)    \
        simcode::Logger::log_write(simcode::LogLevel::LEVEL_INFO,  __FILE__, __LINE__, __func__, fmt, ##args)
#define LOG_WARN(fmt, args...)    \
        simcode::Logger::log_write(simcode::LogLevel::LEVEL_WARN,  __FILE__, __LINE__, __func__, fmt, ##args)
#define LOG_ERROR(fmt, args...)    \
        simcode::Logger::log_write(simcode::LogLevel::LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##args)
#define LOG_FATAL(fmt, args...)    \
        simcode::Logger::log_write(simcode::LogLevel::LEVEL_FATAL, __FILE__, __LINE__, __func__, fmt, ##args

#endif
