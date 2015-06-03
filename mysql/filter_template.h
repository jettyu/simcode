#ifndef FILTER_TEMPLATE_H
#define FILTER_TEMPLATE_H
#include <simcode/mysql/mysql.h>
#include <simcode/mysql/mysql_escape.h>
#include <simcode/base/string_utils.h>
#include <vector>
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

    //嵌套，eg: a.id=b.id
    OBJ& FilterNest(const std::string& name,
                const std::string& value,
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
    OBJ& In(const std::string& name, 
            const std::vector<std::string>& value,
            const std::string& logic=" and ");
    OBJ& In(const std::string& name, 
            const std::vector<uint64_t>& value,
            const std::string& logic=" and ");
    OBJ& Between(const std::string& name,
                 uint64_t value1, 
                 uint64_t value2,
                 const std::string& logic=" and ");
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
    if (flag_) filter_.append(" ").append(logic).append(" ");
    filter_.append(name).append(" ").append(symbol).append(" '").append(escape_(value)).append("'");
    flag_ = true;
    return obj_;
}

template<class OBJ>
inline OBJ& SqlFilter<OBJ>::Filter(const std::string& name,
                                   uint64_t value,
                                   const std::string& logic,
                                   const std::string& symbol)
{
    if (flag_) filter_.append(" ").append(logic).append(" ");
    filter_.append(name).append(" ").append(symbol).append(" ").append(AtoStr(value));
    flag_ = true;
    return obj_;
}

template<class OBJ>
inline OBJ& SqlFilter<OBJ>::FilterNest(const std::string& name,
                                   const std::string& value,
                                   const std::string& logic,
                                   const std::string& symbol)
{
    if (flag_) filter_.append(" ").append(logic).append(" ");
    filter_.append(name).append(" ").append(symbol).append(" ").append(value);
    flag_ = true;
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::Filter(const OBJ& f,
                            const std::string& logic,
                            const std::string& left_symbol,
                            const std::string& right_symbol)
{
    if (flag_) filter_.append(" ").append(logic).append(" ");
    filter_.append(left_symbol).append(f.ToStr()).append(right_symbol);
    flag_ = true;
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::Filter(const std::string& str)
{
    filter_.append(str);
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::OrderBy(const std::string& name)
{
    filter_.append(" order by ").append(name);
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::OrderByDesc(const std::string& name)
{
    filter_.append(" order by ").append(name).append(" desc ");
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::Limit(size_t i)
{
    filter_.append(" limit ").append(AtoStr(i));
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::Limit(size_t i, size_t j)
{
    filter_.append(" limit ").append(AtoStr(i)).append(",").append(AtoStr(j));
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::In(const std::string& name, 
                        const std::vector<std::string>& value,
                        const std::string& logic)
{
    if (value.empty()) return obj_;
    if (flag_) filter_.append(" ").append(logic).append(" ");
    filter_.append(name).append(" in (");
    std::vector<std::string>::const_iterator it;
    for (it=value.begin(); it!=value.end(); ++it)
        filter_.append("'").append(escape_(*it)).append("',");
    filter_.erase(filter_.size()-1);
    filter_.append(")");
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::In(const std::string& name, 
                        const std::vector<uint64_t>& value,
                        const std::string& logic)
{
    if (value.empty()) return obj_;
    if (flag_) filter_.append(" ").append(logic).append(" ");
    filter_.append(name).append(" in (");
    std::vector<std::uint64_t>::const_iterator it;
    for (it=value.begin(); it!=value.end(); ++it)
        filter_.append(AtoStr(*it)).append(",");
    filter_.erase(filter_.size()-1);
    filter_.append(")");
    return obj_;
}

template<class OBJ>
OBJ& SqlFilter<OBJ>::Between(const std::string& name,
                             uint64_t value1, 
                             uint64_t value2,
                             const std::string& logic)
{
    if (flag_) filter_.append(" ").append(logic).append(" ");
    filter_.append(name).append(" between (").append(AtoStr(value1)).append(",").append(AtoStr(value2)).append(")");
    return obj_;
}

}//endof namespace mysql
}//endof namespace sim

#endif
