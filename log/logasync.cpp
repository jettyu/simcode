#include <simcode/log/logasync.h>
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

using namespace simcode;
using namespace log;

LogAsync::LogAsync() :
    isStarted_(false)
{

}

LogAsync::~LogAsync()
{
    isStarted_ = false;
    cond_.notify_one();
    if (thread_) thread_->join();
}


int LogAsync::Init(const std::string& filename, uint64_t rotate_size)
{
    int ret;
    ret = logFile_.open(filename, rotate_size);
    if (ret) return ret;
    thread_.reset(new simex::thread(simex::bind(&LogAsync::Write, this)));
    isStarted_ = true;
    return 0;
}

void LogAsync::Write()
{
    ScopeLock lock(mutex_);
    while(isStarted_)
    {
        lock.unlock();
        if (buffer_.readableBytes() == 0)
        {
            lock.lock();
            buffer_.changeIndex();
            lock.unlock();
        }
        while(buffer_.readableBytes() > 0)
        {
            size_t len = buffer_.readableBytes() < 4096 ? buffer_.readableBytes():4096;
            size_t n = logFile_.logWrite(buffer_.peek(), len);
            //printf("readableBytes=%d|n=%d\n", buffer_.readableBytes(), n);
            if (n <= 0)
            {
                printf("logFile.logWrite failed|ret=%d|size=%d|errno=%d|errmsg=%s\n", 
                        n, len,errno, strerror(errno));
            }
            buffer_.seek(len);
        }
        if (buffer_.readableBytes() == 0)
        {
            buffer_.mutableReadBuf()->clear();
            buffer_.resetSeek();
            lock.lock();
            if (!buffer_.mutableWriteBuf()->empty())
            {
                buffer_.changeIndex();
                continue;
            }
            lock.unlock();
        }
        lock.lock();
        cond_.wait(lock);
    }
}

void LogAsync::log_out(int level,
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
        buffer_.append(buf, len);
        cond_.notify_one();
        //logFile_.logWrite(buf, len);
    }
}

int GlobalLogAsync::Init(const std::string& name, uint64_t rotate_size, int level)
{
    simcode::GlobalLogger::set_level(level);
    LogAsync* logging = new LogAsync;
    if (0 != logging->Init(name, rotate_size))
    {
        delete logging;
        return 1;
    }
    simcode::GlobalLogger::set_base_logger(logging);
    return 0;
}
