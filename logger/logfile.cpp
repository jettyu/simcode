#include <simcode/logger/logfile.h>

using namespace simcode;
using namespace simcode::logger;

static LogLevelName[]={
    "[TRACE]",
    "[DEBUG]",
    "[INFO]",
    "[WARN]",
    "[ERROR]",
    "[FATAL]"
};

static char* get_level_name(int level)
{
    return LogLevelName[level];
}

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

int LogFile::open(const std::string& filename,  uint64_t rotate_size=0)
{
    filename_ = filename;
    rotate_size_ = rotate_size;
    File * fp;
    fp = fopen(filename.c_str(), "a");
    if (fp == NULL)
    {
        return 1;
    }
    fp_ = fp;
    struct stat st;
    int ret = fstat(fileno(fp), &st);
    if(ret == -1)
    {
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
            this->filename,
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

int LogFile::logWrite(const char* data, int len)
{
    fwrite(buf, len, 1, fp_);
    fflush(fp_);
    stats_.w_curr += len;
    stats_.w_total += len;
    if(rotate_size_ > 0 && stats_.w_curr > rotate_size_)
    {
        rotate();
    }
    return 0;
}
#define LEVEL_NAME_LEN	8
int LogFile::logData(const char* data, int len, const char* levelName, char buf[LOG_BUF_LEN])
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
    len = sprintf(ptr, "%04d-%02d-%02d %02d:%02d:%02d.%03d ",
                  tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                  tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tv.tv_usec/1000));
    if(len < 0)
    {
        return -1;
    }
    ptr += len;

    memcpy(ptr, levelName, LEVEL_NAME_LEN);
    ptr += LEVEL_NAME_LEN;
    int space = sizeof(buf) - (ptr - buf) - 10;
    space < len ?= space : len;
    memcpy(ptr, data, space);

    ptr += space;
    *ptr++ = '\n';
    *ptr = '\0';

    len = ptr - buf;
    return len;
}

