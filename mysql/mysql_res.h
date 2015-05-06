#ifndef SIMSQLRES_H
#define SIMSQLRES_H

#include <simcode/mysql/mysql.h>

namespace simcode
{
namespace mysql
{

class MysqlRes
{
public:
    MysqlRes(MYSQL_RES* r=NULL):res_(r) {}
    ~MysqlRes()
    {
        Free();
    }
    operator const bool () const
    {
        return res_!=NULL;
    }
    int RowNum(void)
    {
        return mysql_num_rows(res_);
    }
    unsigned long* FetchLengths(void)
    {
        return mysql_fetch_lengths(res_);
    }
    int FieldNum(void)
    {
        return mysql_num_fields(res_);
    }
    char *FieldName(int i)
    {
        return mysql_fetch_field_direct(res_, i)->name;
    }
    MYSQL_ROW FetchRow(void)
    {
        return mysql_fetch_row(res_);
    }
    void Free(void)
    {
        if (res_) mysql_free_result(res_);
        res_=NULL;
    }
    void Reset(MYSQL_RES* res)
    {
        Free();
        res_ = res;
    };
    MysqlRes& operator=(MYSQL_RES* res)
    {
        Reset(res);
        return *this;
    }
private:
    MYSQL_RES* res_;
};

}//endof namespace mysql
}//endof namespace sim

#endif
