#ifndef MYSQL_COMMON_STR_H
#define MYSQL_COMMON_STR_H

#include <string>
#include <simcode/mysql/mysql.h>
#include <simcode/base/string_utils.h>
#include <simcode/base/typedef.h>

namespace simcode
{
namespace mysql
{

class SqlFilterStr
{
public:
    SqlFilterStr(bool f = false):flag_(f) {}
    bool flag() const
    {
        return flag_;
    }
    SqlFilterStr& add_str(const std::string& name, const std::string& val,
                          const std::string& symbol=" = ",
                          const std::string& logic="and")
    {
        if (flag_) filter_ += " " + logic + " ";
        filter_ += name + symbol + "'" + Mysql::Escape(val)  + "'";
        flag_ = true;
        return *this;
    }
    SqlFilterStr& add_num(const std::string& name, int64_t val,
                          const std::string& symbol=" = ",
                          const std::string& logic="and")
    {
        if (flag_) filter_ += " " + logic + " ";
        filter_ += name + symbol + AtoStr(val);
        flag_ = true;
        return *this;
    }
    SqlFilterStr& add_filter(const SqlFilterStr& filter,
                             const std::string& logic= "and")
    {
        if (flag_) filter_ += " " + logic + " ";
        filter_ += "(" + filter.ToStr() + ")";
        flag_ = true;
        return *this;
    }
    SqlFilterStr& add_filter_str(const std::string& filter_str,
                                 const std::string& logic = "and")
    {
        if (flag_) filter_ += " " + logic + " ";
        flag_ = true;
        filter_ += filter_str;
        return *this;
    }
    SqlFilterStr& add_order(const std::string& name,
                            const std::string& desc="")
    {
        filter_ += " order by " + name + desc;
        return *this;
    }
    SqlFilterStr& add_limit(int64_t i)
    {
        filter_ += " limit " + AtoStr(i);
        return *this;
    }
    SqlFilterStr& add_limit(int64_t i, int64_t j)
    {
        filter_ += " limit " + AtoStr(i) + "," + AtoStr(j);
        return *this;
    }

    const std::string& ToStr() const
    {
        return filter_;
    }
    const std::string& ToString() const
    {
        return filter_;
    }
private:
    std::string filter_;
    bool flag_;
};

class SqlInsertStr
{
public:
    SqlInsertStr(SharedPtr<Mysql> db = NULL):db_(db)
    {
        sqlhead_ = "insert into ";
        sqlbegin_ = "(";
        sqlend_ = ")values(";
    }
    SqlInsertStr& set_table(const std::string& tablename)
    {
        sqlhead_ += tablename;
        return *this;
    }
    SqlInsertStr& add_str(const std::string& name, const std::string& value)
    {
        sqlbegin_ += name + ",";
        sqlend_ += "'" + Mysql::Escape(value) + "',";
        return *this;
    }
    SqlInsertStr& real_add_str(const std::string& name, const std::string& value)
    {
        sqlbegin_ += name + ",";
        sqlend_ += "'" + db_->RealEscape(value) + "',";
        return *this;
    }
    template<typename T>
    SqlInsertStr& add_num(const std::string& name, const T value)
    {
        sqlbegin_ += name + ",";
        sqlend_ += AtoStr(value) + ",";
        return *this;
    }
    const std::string& ToStr(void)
    {
        if (sql_.empty())
        {
            sqlbegin_.erase(sqlbegin_.length()-1);
            sqlend_.erase(sqlend_.length()-1);
            sql_ = sqlhead_ + sqlbegin_ + sqlend_ + ")";
        }
        return sql_;
    }
    operator const std::string &()
    {
        return ToStr();
    }

private:
    std::string sql_;
    std::string sqlhead_;
    std::string sqlbegin_;
    std::string sqlend_;
    SharedPtr<Mysql> db_;
};

class SqlUpdateStr
{
public:
    SqlUpdateStr()
    {
        sqlhead_ = "update ";
        sqlbegin_ = " set ";
        sqlend_ = " where ";
    }
    const std::string& ToStr(void)
    {
        if (sql_.empty())
        {
            sqlbegin_.erase(sqlbegin_.length()-1);
            sql_ = sqlhead_ + sqlbegin_+ sqlend_;
        }
        return sql_;
    }
    operator const std::string &()
    {
        return ToStr();
    }
    SqlUpdateStr& set_table(const std::string& tablename)
    {
        sqlhead_ += tablename;
        return *this;
    }
    SqlUpdateStr& add_str(const std::string& name, const std::string& value)
    {
        sqlbegin_ += name + "='" + Mysql::Escape(value) + "',";
        return *this;
    }
    template<typename T>
    SqlUpdateStr& add_num(const std::string& name, const T value)
    {
        sqlbegin_ += name + "=" + AtoStr(value) + ",";
        return *this;
    }
    SqlUpdateStr& set_filter(const std::string& str)
    {
        sqlend_ += str;
        return *this;
    }

private:
    std::string sql_;
    std::string sqlhead_;
    std::string sqlbegin_;
    std::string sqlend_;
};
}
}

#endif
