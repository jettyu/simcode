#include <simcode/base/simjson.h>
#include <simcode/base/simexception.h>
#include <simcode/base/string_utils.h>
using namespace std;
namespace simcode
{
SimJson SimJson::New(const std::string& str, const int _errcode)
{
    if (str.empty() || !SimJson::is_valid(str))
        throw SimException("str is unvalid json|" + str, _errcode);

    SimJson nsj;
    char b = str[0];
    char e = str[str.length()-1];
    if ('{' == b && '}' == e)
        nsj.m_type = SIMJSON_NODE;
    else if ('[' == b && ']' == e)
        nsj.m_type = SIMJSON_ARRAY;
    else
        throw SimException("str is unvalid json|" + str, _errcode);
    nsj.m_str = str;
    return nsj;
}

SimJson::SimJson(SimJsonNodeType t):m_type(t)
{
    if (SIMJSON_NODE == m_type)
        m_str = "{}";
    else if (SIMJSON_ARRAY == m_type)
        m_str = "[]";
    else m_str="";
}

SimJson::SimJson(const std::string& key, const std::string& value, bool is_transed)
{
    string v = value;
    if (!is_transed) v=StrReplace(value, "\"", "\\\"");
    if (!key.empty())
    {
        m_type = SIMJSON_EMPTY;
        m_str = "\"" + key + "\":\"" + v + "\"";
    }
    else
    {
        m_type = SIMJSON_NODE;
        m_str = "\"" + v + "\"";
    }
}

SimJson::SimJson(const std::string& key, const int value)
{
    char tmp[128]= {0};
    if (!key.empty())
    {
        m_type = SIMJSON_EMPTY;
        static const char* fmt = "\"%s\":%d";
        snprintf(tmp, sizeof(tmp), fmt, key.c_str(), value);
    }
    else
    {
        m_type = SIMJSON_NODE;
        static const char* fmt = "%d";
        snprintf(tmp, sizeof(tmp), fmt, value);
    }
    m_str = tmp;
    //m_str = "\"" + key + "\":" + lltostring(value);
}

SimJson::SimJson(const std::string& key, const SimJson& value)
{
    m_type = SIMJSON_EMPTY;
    m_str = "\"" + key + "\":" + value.m_str;
}

SimJson& SimJson::operator=(const SimJson& sj)
{
    if (this != &sj)
    {
        m_str = sj.m_str;
        m_type = sj.m_type;
    }
    return *this;
}

void SimJson::clear(void)
{
    m_str.clear();
    if (SIMJSON_NODE == m_type) m_str="{}";
    else if (SIMJSON_ARRAY == m_type) m_str="[]";
}

int SimJson::push_back(const SimJson& sj)
{
    if (SIMJSON_EMPTY == m_type)
        return -1;
    if (SIMJSON_NODE == m_type && SIMJSON_EMPTY != sj.m_type)
        return -2;
    else if (SIMJSON_ARRAY == m_type && SIMJSON_NODE != sj.m_type)
        return -3;
    AddNode(m_str, sj.m_str);
    return 0;
}


std::string& SimJson::AddNode(std::string& node, const std::string& str)
{
    size_t len = node.length();
    if (len)
    {
        char e = node[len-1];
        if (len > 2) node[len-1] = ',';
        else node.erase(len-1);
        node.append(str);
        node += e;
    }
    else
    {
        node = str;
    }
    return node;
}

}//end of namespace sim
