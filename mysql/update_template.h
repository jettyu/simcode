#ifndef UPDATE_TEMPLATE_H
#define UPDATE_TEMPLATE_H
#include <simcode/mysql/mysql.h>
#include <simcode/mysql/mysql_escape.h>
#include <simcode/base/string_common.h>
namespace simcode
{
namespace mysql
{

template<class OBJ>
class SqlUpdate
{
public:
    SqlUpdate(OBJ& o, Mysql* s=NULL):obj_(o),escape_(s) {}
    OBJ& setEscape(Mysql* s=NULL)
    {
        escape_.setEscape(s);
        return obj_;
    }
    OBJ& Table(const std::string& table)
    {
        table_=table;
        return obj_;
    }
    OBJ& Update(const std::string& name, const std::string& value);
    OBJ& Update(const std::string& name, uint64_t value);
    const std::string& ToString();

    OBJ& operator() (const std::string& table)
    {
        return Table(table);
    }
    OBJ& operator() (const std::string& name, const std::string& value)
    {
        return Update(name, value);
    }
    OBJ& operator() (const std::string& name, uint64_t value)
    {
        return Update(name, value);
    }

    void Clear()
    {
        table_.clear();
        field_.clear();
    };

private:
    OBJ& obj_;
    Escape escape_;
    std::string table_;
    std::string field_;
    std::string query_;
};

template<class OBJ>
OBJ& SqlUpdate<OBJ>::Update(const std::string& name, const std::string& value)
{
    field_ += name + "='" + escape_(value) + "',";
    return obj_;
}

template<class OBJ>
OBJ& SqlUpdate<OBJ>::Update(const std::string& name, uint64_t value)
{
    field_ += name + "=" + AtoStr(value) + ",";
    return obj_;
}

template<class OBJ>
const std::string& SqlUpdate<OBJ>::ToString()
{
    if (query_.empty() && !field_.empty())
    {
        field_.erase(field_.length()-1);
        query_ = "update " + table_ + " set " + field_;
    }
    return query_;
}

}//end of namespace mysql
}//end of namespace sim

#endif
