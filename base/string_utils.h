#ifndef SIMCODE_STRING_UTILS_H
#define SIMCODE_STRING_UTILS_H
#include <stdint.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <sstream>
namespace simcode
{

inline bool CheckMask(int mask, int off_set)
{
    return mask & (1<<(off_set-1));
}

//读取文件所有内容，转为string
std::string StrLoadFile(const std::string& file);
//字符串批量替换,将s串中str替换成dst,替换count次
std::string StrReplace(const std::string& s,
                       const std::string& str,
                       const std::string& dst,
                       size_t count = -1);
//字符串转小写
std::string StrToLow(char* s, const size_t len);
//字符串转大写
std::string StrToUp(char* s, const size_t len);
int split(const std::string& src, char ch, std::vector<std::string>& dest);
int split(const std::string& src, const std::string& ch, std::vector<std::string>& dest);
//查找c在s中偏移量为pos后第n次出现的位置
template<typename T>
inline int strnfind(const std::string& s, T c, int n, int pos=0)
{
    pos = s.find(c, pos);
    if (n <= 1) return pos;
    return strnfind(s, c, --n, pos+1);
}

//整数转字符串
inline std::string itostr(uint64_t i)
{
#ifndef MAX_INT_LENGH
#define MAX_INT_LENGH 20
#endif
    char data[MAX_INT_LENGH];
    snprintf(data, MAX_INT_LENGH, "%llu", i);
    return data;
}

template<typename T>
inline std::string atostr(T i)
{
    return atostr<uint64_t>(i);
}

template<>
inline std::string atostr(uint64_t i)
{
#ifndef MAX_INT_LENGH
#define MAX_INT_LENGH 20
#endif
    char data[MAX_INT_LENGH];
    snprintf(data, MAX_INT_LENGH, "%llu", i);
    return data;
}

template<>
inline std::string atostr(double i)
{
    std::ostringstream o;
    o << i;
    return o.str();
}

template<>
inline std::string atostr(float i)
{
    std::ostringstream o;
    o << i;
    return o.str();
}

template<typename T>
inline std::string AtoStr(T i)
{
    return atostr(i);
}


template<typename T>
T strtoa(const char* src,
         const std::string& sDefault="")
{
    std::string sStr;
    if (!src) sStr = src;
    const std::string *s;
    if(!sStr.empty()) s = &sStr;
    else s = &sDefault;
    std::istringstream sBuffer(*s);
    T t;
    sBuffer >> t;
    return t;
}
template<typename T>
T strtoa(const std::string& sStr,
         const std::string& sDefault="")
{
    const std::string *s;
    if(!sStr.empty()) s = &sStr;
    else s = &sDefault;
    std::istringstream sBuffer(*s);
    T t;
    sBuffer >> t;
    return t;
}

}
#endif
