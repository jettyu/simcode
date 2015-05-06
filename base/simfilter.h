#ifndef SIM_FILTER_H
#define SIM_FILTER_H

#include <string>
#include <map>
#include <vector>
#include <set>
#include <iostream>

namespace simcode
{

class Filter
{
public:
    virtual bool Exec(void *parmer)
    {
        return false;
    }
};

struct FilterUtil
{
    FilterUtil():filter(NULL) {}
    Filter* filter;
};

typedef std::map<std::string, FilterUtil*> MarkFilterUtilMap;
typedef std::vector<FilterUtil*> FilterVec;

template<typename T>
class FilterManager
{
public:
    bool Exec(const T& condition,
              void *parmer,
              bool is_excat_filted = false)
    {
        std::cout<<"is_excat_filted="<<is_excat_filted<<std::endl;
        if (is_excat_filted) return ExactFilter(condition, parmer);
        return FetchFilter(condition, parmer);
    }
    //condition完全匹配
    bool ExactFilter(const T& condition, void *parmer);
    //遍历所有符合条件的过滤器，只有一个不能通过则直接返回false
    bool FetchFilter(const T& condition, void *parmer);

    bool Register(const std::string& mark, Filter* f);
    void AddCondition(const T& condition, const std::string& mark);
    void Destroy();
protected:
    //判断条件是否符合，FetchFilter中使用,由使用者自行实现
    virtual bool FitCondition(T& pre_cond, T& new_cond)
    {
        return false;
    }
private:
//标识 - FilterUtil
    MarkFilterUtilMap mark_util_;
//条件 - FilterUtils
    std::map<T, FilterVec> condition_util_;
//用于记录所有new出来的filter,释放时使用
    std::set<Filter*> filter_set_;
};

class UrlFilterManager : public FilterManager<std::string>
{
public:
    virtual bool FitCondition(std::string& pre_url, std::string& new_url)
    {
        if (pre_url.length() == new_url.length()) return false;
        size_t pos = pre_url.find("/", new_url.length());
        if (pos == std::string::npos) new_url = pre_url;
        else new_url = pre_url.substr(0, pos+1);
        std::cout<<"pre_url="<<pre_url<<std::endl;
        std::cout<<"new_url="<<new_url<<std::endl;
        return true;
    }
};

template<typename T>
bool FilterManager<T>::ExactFilter(const T& condition, void *parmer)
{
    std::cout<<"condition="<<condition<<std::endl;
    typename std::map<T, FilterVec>::iterator it;
    it = condition_util_.find(condition);
    if (it != condition_util_.end())
    {
        FilterVec::iterator vit;
        for (vit=it->second.begin(); vit!=it->second.end(); ++vit)
        {
            if ((*vit)->filter && !(*vit)->filter->Exec(parmer)) return false;
        }
    }
    return true;
}

template<typename T>
bool FilterManager<T>::FetchFilter(const T& condition, void *parmer)
{
    T c = condition;
    T new_c = "";
    while (FitCondition(c, new_c))
        if(!ExactFilter(new_c, parmer)) return false;
    return true;
}

template<typename T>
bool FilterManager<T>::Register(const std::string& mark, Filter* f)
{
    FilterUtil* util = NULL;
    MarkFilterUtilMap::iterator it;
    it = mark_util_.find(mark);
    if (it == mark_util_.end()) util = new FilterUtil;
    else util = it->second;
    if (util->filter) return false;
    util->filter = f;
    mark_util_[mark] = util;
    std::cout<<"mark="<<mark<<std::endl;
    filter_set_.insert(f);
    return true;
}

template<typename T>
void FilterManager<T>::AddCondition(const T& condition, const std::string& mark)
{
    FilterUtil* util = NULL;
    MarkFilterUtilMap::iterator it;
    it = mark_util_.find(mark);
    if (it == mark_util_.end())
    {
        util = new FilterUtil;
        mark_util_[mark] = util;
    }
    else
    {
        util = it->second;
    }
    std::cout<<"mark="<<mark<<std::endl;
    std::cout<<"condition="<<condition<<std::endl;
    condition_util_[condition].push_back(util);
}

template<typename T>
void FilterManager<T>::Destroy()
{
    std::set<Filter*>::iterator sit;
    for (sit=filter_set_.begin(); sit!=filter_set_.end(); ++sit)
        delete *sit;
    filter_set_.clear();
    MarkFilterUtilMap::iterator mit;
    for (mit=mark_util_.begin(); mit!=mark_util_.end(); ++mit)
        delete mit->second;
    mark_util_.clear();
    condition_util_.clear();
}

}

#endif
