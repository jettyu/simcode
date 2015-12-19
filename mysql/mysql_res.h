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
    operator bool () const
    {
        return res_!=NULL;
    }
    MYSQL_RES* operator->() const
    {
        return res_;
    }
    void reset(MYSQL_RES* res)
    {
        Free();
        res_ = res;
    };
    MysqlRes& operator=(MYSQL_RES* res)
    {
        reset(res);
        return *this;
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
    MYSQL_FIELD* Fields()
    {
        return res_->fields;
    }
    MYSQL_FIELD *Field(int i)
    {
        return mysql_fetch_field_direct(res_, i);
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
private:
    MysqlRes( const MysqlRes& ) {}
    MysqlRes& operator=( const MysqlRes& )
    {
        return *this;
    }
private:
    MYSQL_RES* res_;
};

}//endof namespace mysql
}//endof namespace sim

#endif
