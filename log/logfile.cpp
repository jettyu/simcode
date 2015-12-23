#include <simcode/log/logfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <simcode/base/logger.h>
//just for c++11
#include <simcode/base/typedef.h>
using namespace simcode;
using namespace simcode::log;

LogFile::LogFile()
{

}

LogFile::~LogFile()
{
    close();
}

int LogFile::open(FILE *fp)
{
    fp_ = fp;
    return 0;
}

int LogFile::open(const std::string& filename,  uint64_t rotate_size)
{
    filename_ = filename;
    rotate_size_ = rotate_size;
    FILE * fp;
    fp = fopen(filename.c_str(), "a");
    if (fp == NULL)
    {
        LOG_ERROR("error=%s\n", strerror(errno));
        return 1;
    }
    fp_ = fp;
    struct stat st;
    int ret = ::fstat(fileno(fp), &st);
    if(ret == -1)
    {
        LOG_ERROR("error=%s\n", strerror(errno));
        return 1;
    }
    else
    {
        stats_.w_curr = st.st_size;
    }
    return 0;
}

void LogFile::close()
{
    if (fp_ != NULL)
    {
        fclose(fp_);
    }
}

void LogFile::rotate()
{
    close();
    char newpath[PATH_MAX];
    time_t time;
    struct timeval tv;
    struct tm *tm;
    gettimeofday(&tv, NULL);
    time = tv.tv_sec;
    tm = localtime(&time);
    sprintf(newpath, "%s.%04d%02d%02d-%02d%02d%02d",
            filename_.c_str(),
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

    //printf("rename %s => %s\n", this->filename, newpath);
    int ret = rename(filename_.c_str(), newpath);
    if(ret == -1)
    {
        return;
    }
    fp_ = fopen(filename_.c_str(), "a");
    if(fp_ == NULL)
    {
        return;
    }
    stats_.w_curr = 0;
}

int LogFile::logWrite(const char* data, int size)
{
    ::fwrite(data, size, 1, fp_);
    ::fflush(fp_);
    stats_.w_curr += size;
    stats_.w_total += size;
    if(rotate_size_ > 0 && stats_.w_curr > rotate_size_)
    {
        rotate();
    }
    return 0;
}

int LogFile::logWriteOnly(const char* data, int size)
{
    int n = ::fwrite(data, size, 1, fp_);
    if (n > 0)
    {
        stats_.w_curr += n;
        stats_.w_total += n;
    }
    return n;
}

int LogFile::logFflush()
{
    int ret = ::fflush(fp_);
    if (ret == 0)
    {
        if(rotate_size_ > 0 && stats_.w_curr > rotate_size_)
        {
            rotate();
        }
    }
    return ret;
}

#define LEVEL_NAME_LEN	8
int LogFile::logData(const char* data, int size, const char* levelName, char buf[LOG_BUF_LEN])
{
    int len;
    char *ptr = buf;

    time_t time;
    struct timeval tv;
    struct tm *tm;
    gettimeofday(&tv, NULL);
    time = tv.tv_sec;
    tm = localtime(&time);
    /* %3ld 在数值位数超过3位的时候不起作用, 所以这里转成int */
    len = snprintf(ptr, LOG_BUF_LEN, "%04d-%02d-%02d %02d:%02d:%02d.%03d |%llu|",
                   tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                   tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec/1000),
                   cur_thread_id());
    if(len < 0)
    {
        return -1;
    }
    ptr += len;
    int levelNameLen = strlen(levelName);
    memcpy(ptr, levelName, levelNameLen);
    ptr += levelNameLen;
    int space = LOG_BUF_LEN - (ptr - buf) - 10;
    space = space < size ? space : size;
    memcpy(ptr, data, space);

    ptr += space;
    *ptr++ = '\n';
    *ptr = '\0';

    len = ptr - buf;
    return len;
}

