#ifndef SIMCODE_NET_NETNETLOGGER_H
#define SIMCODE_NET_NETNETLOGGER_H
#include <stdarg.h>
namespace simcode
{
namespace net
{

class NetLogLevel
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

class NetLogger
{
public:
    //日志级别，文件名，文件行号，日志内容
    typedef void (*log_func_t)(int level, 
                               const char* filename, 
                               int linenum, 
                               const char* msg);
    static void set_level(int l);
    static void set_log_fun(log_func_t f);
    static void log_write(int level, 
                         const char* filename, 
                         int linenum, 
                         const char *fmt, ...);
private:
    static void log_out(int level, 
                        const char* filename, 
                        int linenum, 
                        const char* msg);
    static void log_out_valist(int level, 
                               const char* filename, 
                               int linenum, 
                               const char*fmt, 
                               va_list ap);
private:
    static int level_;
    static log_func_t log_func_;
};

#define NETLOG_DEBUG(fmt, args...)    \
        NetLogger::log_write(NetLogLevel::LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##args)
#define NETLOG_INFO(fmt, args...)    \
        NetLogger::log_write(NetLogLevel::LEVEL_INFO,  __FILE__, __LINE__, fmt, ##args)
#define NETLOG_WARN(fmt, args...)    \
        NetLogger::log_write(NetLogLevel::LEVEL_WARN,  __FILE__, __LINE__, fmt, ##args)
#define NETLOG_ERROR(fmt, args...)    \
        NetLogger::log_write(NetLogLevel::LEVEL_ERROR, __FILE__, __LINE__, fmt, ##args)
#define NETLOG_FATAL(fmt, args...)    \
        NetLogger::log_write(NetLogLevel::LEVEL_FATAL, __FILE__, __LINE__, fmt, ##args
}
}

#endif
