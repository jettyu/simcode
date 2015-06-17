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
    inline command& operator<<(const T arg)
    {
        ostream_.str("");
        ostream_<<arg;
        args_.push_back(ostream_.str());
        return *this;
    }

    template<typename T>
    inline command& operator()(const T arg)
    {
        ostream_.str("");
        ostream_<<arg;
        args_.push_back(ostream_.str());
        return *this;
    }

    inline operator const std::vector<std::string>& () const
    {
        return args_;
    }

    inline std::vector<std::string>* operator-> ()
    {
        return &args_;
    }

private:
    std::ostringstream ostream_;
    std::vector<std::string> args_;
};

template<>
inline command& command::operator<<(const char* arg)
{
    args_.push_back(arg);
    return *this;
}

template<>
inline command& command::operator<<(const std::string& arg)
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
inline command& command::operator()(const std::string& arg)
{
    args_.push_back(arg);
    return *this;
}

}
#endif
