#include <simcode/base/logger.h>
using namespace simcode;

int LogLevel::LEVEL_TRACE = 1;
int LogLevel::LEVEL_DEBUG = 2;
int LogLevel::LEVEL_INFO  = 3;
int LogLevel::LEVEL_WARN  = 4;
int LogLevel::LEVEL_ERROR = 5;
int LogLevel::LEVEL_FATAL = 6;

Logger GlobalLogger::logger_;

Logger::Logger():
    level_(LogLevel::LEVEL_DEBUG),
    base_logger_(new BaseLogger)
{
}

Logger::~Logger()
{
    if (base_logger_) delete base_logger_;
}

void Logger::set_level(int l)
{
    level_ = l;
}

void Logger::set_base_logger(BaseLogger* b)
{
    if (base_logger_) delete base_logger_;
    base_logger_ = b;
}

void Logger::log_write(int level,
                       const char* filename,
                       int linenum,
                       const char* funcname,
                       const char* fmt, ...)
{
    if (base_logger_->check_level(level_, level)) return;
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
    base_logger_->log_out(level, filename, linenum, funcname, buf);
}

