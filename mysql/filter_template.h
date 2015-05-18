#ifndef FILTER_TEMPLATE_H
#define FILTER_TEMPLATE_H
#include <simcode/mysql/mysql.h>
#include <simcode/mysql/mysql_escape.h>
#include <simcode/base/string_utils.h>
namespace simcode
{

namespace mysql
{

template<class OBJ>
class SqlFilter
{
public:
    SqlFilter(OBJ& o, Mysql* s=NULL):obj_(o),escape_(s),flag_(false) {}
    OBJ& setEscape(Mysql*s = NULL)
    {
        escape_.setEscape();
        return obj_;
    }
    OBJ& FilterFlag (bool f)
    {
        flag_=f;
        return obj_;
    }
    bool FilterFlag () const
    {
        return flag_;
    }
    OBJ& Filter(const std::string& name,
                const std::string& value,
                const std::string& logic="and",
                const std::string& symbol="="
               );
    OBJ& Filter(const std::string& name,
                uint64_t value,
                const std::string& logic="and",
                const std::string& symbol="="
               );

    OBJ& Filter(const OBJ& f,
                const std::string& logic="and",
                const std::string& left_symbol="(",
                const std::string& right_symbol=")");
    OBJ& Filter(const std::string& str);
    OBJ& OrderBy(const std::string& name);
    OBJ& OrderByDesc(const std::string& name);
    OBJ& Limit(size_t i);
    OBJ& Limit(size_t i, size_t j);
    OBJ& ClearFilter()
    {
        filter_.clear();
        flag_=false;
        return obj_;
    }
    const std::string& ToString() const
    {
        return filter_;
    }

private:
    OBJ& obj_;
    Escape escape_;
    std::string filter_;
    bool flag_;
};

class MysqlFilter : public SqlFilter<MysqlFilter>
{
public:
    MysqlFilter(Mysql* s=NULL) : SqlFilter<MysqlFilter>(*this, s)
    {
    }
};

template<class OBJ>
inline OBJ& SqlFilter<OBJ>::Filter(const std::string& name,
                                   const std::string& value,
                                   const std::string& logic,
                                   const std::string& symbol)
{
    if (flag_) filter_ += " " + logic + " ";
    filter_ += name + " " + symbol + " '" + escape_(value)  + "'";
    flag_ = true;
    return obj_;
}

template<class OBJ>
inline OBJ& SqlFilter<OBJ>::Filter(const std::string& name,
                                   uint64_t value,
                                   const std::string& logic,
                                   const std::string& symbol)
{
    if (flag_) filter_ += " " + logic + " ";
    filter_ += name + " " + symbol + " " + AtoStr(value);
    flag_ = true;
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::Filter(const OBJ& f,
                            const std::string& logic,
                            const std::string& left_symbol,
                            const std::string& right_symbol)
{
    if (flag_) filter_ += " " + logic + " ";
    filter_ += left_symbol + f.ToStr() + right_symbol;
    flag_ = true;
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::Filter(const std::string& str)
{
    filter_ += str;
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::OrderBy(const std::string& name)
{
    filter_ += " order by " + name;
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::OrderByDesc(const std::string& name)
{
    filter_ += " order by " + name + " desc";
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::Limit(size_t i)
{
    filter_ += " limit " + AtoStr(i);
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::Limit(size_t i, size_t j)
{
    filter_ += " limit " + AtoStr(i) + "," + AtoStr(j);
    return obj_;
}

}//endof namespace mysql
}//endof namespace sim

#endif
