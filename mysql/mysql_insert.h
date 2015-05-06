#ifndef SIMMYSQL_INSERT_H
#define SIMMYSQL_INSERT_H
#include <simcode/mysql/insert_template.h>

namespace simcode
{
namespace mysql
{

class MysqlInsert : public SqlInsert<MysqlInsert>
{
public:
    MysqlInsert(Mysql* s=NULL):SqlInsert<MysqlInsert>(*this, s),sql_(s)
    {
    }
    MysqlInsert& Use(const std::string& db)
    {
        sql_->TryQuery("use " + db);
        return *this;
    }
    int Query()
    {
        return sql_->Query(ToString());
    }
    int TryQuery()
    {
        return sql_->TryQuery(ToString());
    }
private:
    Mysql* sql_;
};

}//end of namespace mysql
}//end of namespace sim
#endif
