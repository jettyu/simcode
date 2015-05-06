#ifndef MYSQL_FILTER_H
#define MYSQL_FILTER_H
#include <simcode/mysql/filter_template.h>
namespace simcode
{

namespace mysql
{

class MysqlFilter : public SqlFilter<MysqlFilter>
{
public:
    MysqlFilter(Mysql* s=NULL):SqlFilter<MysqlFilter>(*this, s)
    {
    }
};

}//endof namespace mysql
}//endof namespace sim

#endif
