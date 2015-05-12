#include <simcode/net/NetLogger.h>
#include <stdio.h>
using namespace simcode;
using namespace net;

int NetLogLevel::LEVEL_DEBUG = 1;
int NetLogLevel::LEVEL_INFO  = 2;
int NetLogLevel::LEVEL_WARN  = 3;
int NetLogLevel::LEVEL_ERROR = 4;
int NetLogLevel::LEVEL_FATAL = 5;

int NetLogger::level_ = NetLogLevel::LEVEL_DEBUG;
NetLogger::log_func_t NetLogger::log_func_ = NetLogger::log_out;

void NetLogger::set_level(int l)
{
    level_ = l;
}

void NetLogger::set_log_fun(log_func_t f)
{
    log_func_ = f;
}

void NetLogger::log_write(int level, 
                          const char* filename, 
                          int linenum, 
                          const char* fmt, ...)
{
    if (level < level_) return;
    va_list ap;
    va_start(ap, fmt);
    log_out_valist(level, filename, linenum, fmt, ap);
    va_end(ap);
}

#define LOG_BUF_LEN       4096
void NetLogger::log_out_valist(int level,
                        const char* filename,
                        int linenum,
                        const char* fmt,
                        va_list ap)
{
    char buf[LOG_BUF_LEN];
    vsnprintf(buf, LOG_BUF_LEN, fmt, ap);
    log_func_(level, filename, linenum, buf);
}


void NetLogger::log_out(int level, 
                        const char* filename, 
                        int linenum, 
                        const char* msg)
{
    printf("log_level=%d|%s|%d|%s\n", level, filename, linenum, msg);
}
