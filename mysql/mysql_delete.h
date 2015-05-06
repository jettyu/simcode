#ifndef SIMCODE_MYSQL_DELETE_H
#define SIMCODE_MYSQL_DELETE_H
namespace simcode
{
namespace mysql
{

class MysqlDelete : public SqlFilter<MysqlDelete>
{
public:
    MysqlDelete(Mysql* s=NULL):
        SqlFilter<MysqlDelete>(*this, s), sql_(s)
    {
    }
    MysqlDelete& Table(const std::string& table)
    {
        table_ = table;
        return *this;
    }
    const std::string& ToString()
    {
        if (query_.empty()) query_ = "delete from " + table_ + " where " + SqlFilter<MysqlDelete>::ToString();
        return query_;
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
    std::string table_;
    std::string query_;
};

}
}
#endif
