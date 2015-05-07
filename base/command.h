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
        osteam_.clear();
        osteam_<<i;
        return osteam_.str();
    }
    
    std::vector<std::string> args_;
    std::ostringstream osteam_;
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

}
#endif
