#ifndef SIMJSON_H
#define SIMJSON_H

#include <string>

namespace simcode
{

enum SimJsonNodeType
{
    SIMJSON_NODE = 1,
    SIMJSON_ARRAY,
    SIMJSON_EMPTY
};
class SimJson;
inline SimJson NewSimJson(const std::string& str, const int _errcode=0);

class SimJson
{
public:
    static SimJson New(const std::string& str, const int _errcode=0);

    static bool is_valid(const std::string& str)
    {
        return true;
    }
    const std::string& ToString()const
    {
        return m_str;
    }

    SimJson():m_type(SIMJSON_EMPTY) {}
    SimJson(SimJsonNodeType t);
    //is_transed标记是否转移，如果为false则表示需要转译
    SimJson(const std::string& key, const std::string& value, bool is_transed = false);
    SimJson(const std::string& key, const int value);

    SimJson(const std::string& key, const SimJson& value);

    SimJson& operator=(const SimJson& sj);


    void reserve(const size_t n)
    {
        m_str.reserve(n);
    }
    size_t size(void)
    {
        return m_str.length();
    }
    size_t capacity(void)
    {
        return m_str.capacity();
    }
    void clear(void);
    template<typename T>
    int push_back(const std::string& key, const T& val, bool is_transed = false)
    {
        return push_back(SimJson(key, val));
    }
    int push_back(const std::string& key, const std::string& val, bool is_transed = false)
    {
        return push_back(SimJson(key, val, is_transed));
    }
    int push_back(const SimJson& sj);
//会自动转译字符串
    SimJsonNodeType type(void)
    {
        return m_type;
    }

private:
    std::string& AddNode(std::string& node, const std::string& str);
private:
    std::string m_str;
    SimJsonNodeType m_type;
};//endof class

inline SimJson NewSimJson(const std::string& str)
{
    return SimJson::New(str);
}

};//endof namespace
#endif
