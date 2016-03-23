#ifndef SIMEXCEPTION_H
#define SIMEXCEPTION_H
#include <string>
namespace simcode
{
class SimException
{
public:
    SimException(const std::string& m="", const int c=0)
        :code_(c), msg_(m) {}
    inline const int code()const
    {
        return code_;
    }
    inline const std::string& msg()const
    {
        return msg_;
    }
    inline const char* what() const
    {
        return msg_.c_str();
    }
    inline void set_code(const int c)
    {
        code_=c;
    }
    inline void set_msg(const std::string& m)
    {
        msg_=m;
    }
private:
    int code_;
    std::string msg_;
};
};
#endif
