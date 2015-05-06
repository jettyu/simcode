#ifndef QUERY_SECTOR_H
#define QUERY_SECTOR_H
#include <simcode/mysql/mysql_select.h>
#include <simcode/mysql/mysql_insert.h>
#include <simcode/mysql/mysql_update.h>
#include <simcode/mysql/mysql_delete.h>

namespace simcode
{
namespace mysql
{

class QuerySector : public Mysql
{
public:
    QuerySector():select_(this),insert_(this),update_(this) {}
    MysqlSelect& Select()
    {
        return select_;
    }
    MysqlInsert& Insert()
    {
        return insert_;
    }
    MysqlUpdate& Update()
    {
        return update_;
    }
    MysqlDelete& Delete()
    {
        return delete_;
    }
private:
    MysqlSelect select_;
    MysqlInsert insert_;
    MysqlUpdate update_;
    MysqlDelete delete_;
};

}//endof namespace mysql
}//endof namespace sim

#endif
