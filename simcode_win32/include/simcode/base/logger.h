#ifndef SIMCODE_LOGGER_H
#define SIMCODE_LOGGER_H
#include <stdarg.h>
#include <stdio.h>
namespace simcode
{

class LogLevel
{
public:
    static void Init(int trace = 1,
                     int debug=2,
                     int info=3,
                     int warn=4,
                     int error=5,
                     int fatal=6)
    {
        LEVEL_TRACE = trace;
        LEVEL_DEBUG = debug;
        LEVEL_INFO = info;
        LEVEL_WARN = warn;
        LEVEL_ERROR = error;
        LEVEL_FATAL = fatal;
    }
    static int LEVEL_TRACE;
    static int LEVEL_DEBUG;
    static int LEVEL_INFO;
    static int LEVEL_WARN;
    static int LEVEL_ERROR;
    static int LEVEL_FATAL;
};

class BaseLogger
{
public:
     //日志级别，文件名，文件行号，日志内容
    virtual void log_out(int level,
                        const char* filename,
                        int linenum,
                        const char* funcname,
                        const char* msg)
    {
        printf("log_level=%d|%s|%d|%s|%s\n", level, filename, linenum, funcname, msg);
    }
    virtual bool check_level(int setlevel, int curlevel)
    {
        return curlevel >= setlevel;
    }
};

class Logger
{
public:
    Logger();
    ~Logger();
    void set_level(int l);
    void set_base_logger(BaseLogger* b);
    void log_write(int level,
                   const char* filename,
                   int linenum,
                   const char* funcname,
                   const char *fmt, ...);
private:
    void log_out_valist(int level,
                        const char* filename,
                        int linenum,
                        const char* funcname,
                        const char*fmt,
                        va_list ap);
private:
    int level_;
    BaseLogger* base_logger_;
};

class GlobalLogger
{
public:
    static void set_level(int l)
    {
        logger_.set_level(l);
    }
    static void set_base_logger(BaseLogger* f)
    {
        logger_.set_base_logger(f);
    }
    static Logger& logger()
    {
        return logger_;
    }
private:
    static Logger logger_;
};

}

#ifndef _MSC_VER  
#define _MSC_VER 1600 
#endif 

#define LOG_TRACE(fmt,...)    \
        simcode::GlobalLogger::logger().log_write(simcode::LogLevel::LEVEL_TRACE, __FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt,...)    \
        simcode::GlobalLogger::logger().log_write(simcode::LogLevel::LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOG_INFO(fmt,...)    \
        simcode::GlobalLogger::logger().log_write(simcode::LogLevel::LEVEL_INFO,  __FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOG_WARN(fmt,...)    \
        simcode::GlobalLogger::logger().log_write(simcode::LogLevel::LEVEL_WARN,  __FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt,...)    \
        simcode::GlobalLogger::logger().log_write(simcode::LogLevel::LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOG_FATAL(fmt,...)    \
        simcode::GlobalLogger::logger().log_write(simcode::LogLevel::LEVEL_FATAL, __FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__)

#endif
