#ifndef MYSQL_SELECTOR_H
#define MYSQL_SELECTOR_H
#include <simcode/mysql/filter_template.h>
#include <simcode/mysql/mysql_res.h>
namespace simcode
{
namespace mysql
{

class MysqlSelect : public SqlFilter<MysqlSelect>
{
public:
    MysqlSelect(Mysql* s=NULL):SqlFilter<MysqlSelect>(*this, s),sql_(s)
    {
    }
    MysqlRes Query();
    MysqlRes TryQuery();
    MysqlSelect& Select(const std::string& s);
    MysqlSelect& Table(const std::string& table)
    {
        table_=table;
        return *this;
    }
    MysqlSelect& Use(const std::string& db)
    {
        sql_->TryQuery("use " + db);
        return *this;
    }
    MysqlSelect& operator() (const std::string& s)
    {
        return Select(s);
    }
    const std::string& ToString()
    {
        if (query_.empty())
            query_ = "select " + selector_ + " from " + table_ + " where " + SqlFilter<MysqlSelect>::ToString();
        return query_;
    }
    void Clear();
private:
    Mysql* sql_;
    std::string selector_;
    std::string table_;
    std::string query_;
};

inline MysqlRes MysqlSelect::Query()
{
    return sql_->QueryRes(ToString());
}

inline MysqlRes MysqlSelect::TryQuery()
{
    return sql_->TryQueryRes(ToString());
}

inline MysqlSelect& MysqlSelect::Select(const std::string& s)
{
    selector_ = s;
    return *this;
}

inline void MysqlSelect::Clear()
{
    selector_.clear();
}

}
}

#endif
