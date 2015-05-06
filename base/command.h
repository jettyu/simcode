#ifndef SIMCODE_COMMAND_H
#define SIMCODE_COMMAND_H

#include <string>
#include <vector>
#include <stdio.h>
#include <sstream>
namespace simcode
{

class command
{
public:
    inline command() {}

    inline command(const std::string& arg)
    {
        args_.push_back(arg);
    }

    template<typename T>
    inline command& operator<<(T arg)
    {
        args_.push_back(atostr(arg));
        return *this;
    }

    template<typename T>
    inline command& operator()(T arg)
    {
        args_.push_back(atostr(arg));
        return *this;
    }
    
    inline operator const std::vector<std::string>& ()
    {
        return args_;
    }

    inline std::vector<std::string>* operator-> ()
    {
        return &args_;
    }

private:
    template<typename T>
    std::string atostr(T i)
    {
        atostr<uint64_t>(i);
    }
    
    std::vector<std::string> args_;
};

template<>
inline command& command::operator<<(const std::string& arg)
{
    args_.push_back(arg);
    return *this;
}
template<>
inline command& command::operator<<(const char* arg)
{
    args_.push_back(arg);
    return *this;
}

template<>
inline command& command::operator()(const std::string& arg)
{
    args_.push_back(arg);
    return *this;
}
template<>
inline command& command::operator()(const char* arg)
{
    args_.push_back(arg);
    return *this;
}

template<>
std::string command::atostr(uint64_t i)
{
#ifndef MAX_INT_LENGH
#define MAX_INT_LENGH 21
#endif
    char data[MAX_INT_LENGH]= {0};
    sprintf(data, "%llu", i);
    return data;
}
template<>
std::string command::atostr(double i)
{
    std::ostringstream o;
    if (!(o << i))
    return "";
    return o.str();
}
template<>
std::string command::atostr(float i)
{
    std::ostringstream o;
    if (!(o << i))
    return "";
    return o.str();
}


}
#endif
