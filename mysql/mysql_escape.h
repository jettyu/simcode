#ifndef MYSQL_ESCAPE_H
#define MYSQL_ESCAPE_H
#include <simcode/mysql/mysql.h>

namespace simcode
{

namespace mysql
{

class Escape
{
public:
    Escape(Mysql* s=NULL):sql_(s) {}
    void setEscape(Mysql* s=NULL)
    {
        sql_ = s;
    }
    std::string operator() (const std::string& str)
    {
        if (sql_) return sql_->RealEscape(str);
        else return Mysql::Escape(str);
    }
private:
    Mysql* sql_;
};

}

}

#endif
