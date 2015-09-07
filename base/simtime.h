#ifndef SIMCODE_SIMTIME_H
#define SIMCODE_SIMTIME_H
#include <sys/time.h>
#include <time.h>

namespace simcode
{

#define MIN 60
#define HOUR 3600
#define DAY 86400

inline int get_day_sec(time_t ti)
{
    struct tm* t = localtime(&ti);
    int offset = t->tm_sec + t->tm_min*MIN + t->tm_hour*HOUR;
    return ti - offset;
}

class SimTimeGet
{
public:
    SimTimeGet()
    {
        gettimeofday(&m_st, NULL);
    }
    inline int64_t msec()
    {
        return (int64_t(m_st.tv_sec)*1000 + int64_t(m_st.tv_usec)/1000);
    }
    //时间差
    inline int  msec_interval()
    {
        gettimeofday(&m_et, NULL);
        return (int64_t(m_et.tv_sec)*1000 + int64_t(m_et.tv_usec)/1000) - (int64_t(m_st.tv_sec)*1000 + int64_t(m_st.tv_usec)/1000);
    }
    inline void reset()
    {
        gettimeofday(&m_st, NULL);
    }
private:
    struct timeval m_st;
    struct timeval m_et;
};
}
#endif
