#ifndef MYSQL_UPDATE_H
#define MYSQL_UPDATE_H
#include <simcode/mysql/filter_template.h>
#include <simcode/mysql/update_template.h>
namespace simcode
{
namespace mysql
{
class MysqlUpdate : public SqlFilter<MysqlUpdate>
{
public:
    SqlUpdate<MysqlUpdate> Update;
public:
    MysqlUpdate(Mysql* s=NULL):SqlFilter<MysqlUpdate>(*this, s), Update(*this, s), sql_(s)
    {
    }
    MysqlUpdate& setEscape(Mysql* s=NULL)
    {
        Update.setEscape(s);
        SqlFilter<MysqlUpdate>::setEscape(s);
        return *this;
    }
    const std::string& ToString()
    {
        if (query_.empty()) query_ = Update.ToString() + " where " + SqlFilter<MysqlUpdate>::ToString();
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
    MysqlUpdate& operator() (const std::string& table)
    {
        Update(table);
        return *this;
    }
    MysqlUpdate& Table(const std::string& table)
    {
        Update(table);
        return *this;
    }
    MysqlUpdate& Use(const std::string& db)
    {
        sql_->Query("use " + db);
        return *this;
    }
private:
    Mysql* sql_;
    std::string query_;
};

}
}

#endif
