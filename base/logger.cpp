#include <simcode/base/logger.h>
#include <stdio.h>
using namespace simcode;

int LogLevel::LEVEL_DEBUG = 1;
int LogLevel::LEVEL_INFO  = 2;
int LogLevel::LEVEL_WARN  = 3;
int LogLevel::LEVEL_ERROR = 4;
int LogLevel::LEVEL_FATAL = 5;

int Logger::level_ = LogLevel::LEVEL_DEBUG;
Logger::log_func_t Logger::log_func_ = Logger::log_out;

void Logger::set_level(int l)
{
    level_ = l;
}

void Logger::set_log_fun(log_func_t f)
{
    log_func_ = f;
}

void Logger::log_write(int level, 
                          const char* filename, 
                          int linenum, 
                          const char* funcname,
                          const char* fmt, ...)
{
    if (level < level_) return;
    va_list ap;
    va_start(ap, fmt);
    log_out_valist(level, filename, linenum, funcname, fmt, ap);
    va_end(ap);
}

#define LOG_BUF_LEN       4096
void Logger::log_out_valist(int level,
                        const char* filename,
                        int linenum,
                        const char* funcname,
                        const char* fmt,
                        va_list ap)
{
    char buf[LOG_BUF_LEN];
    vsnprintf(buf, LOG_BUF_LEN, fmt, ap);
    log_func_(level, filename, linenum, funcname, buf);
}


void Logger::log_out(int level, 
                        const char* filename, 
                        int linenum, 
                        const char* funcname,
                        const char* msg)
{
    printf("log_level=%d|%s|%d|%s|%s\n", level, filename, linenum, funcname, msg);
}
