#include <iostream>
#include <fstream>
using namespace std;
#include <stdlib.h>
#include <ctype.h>

#include <simcode/base/simini.h>

namespace simcode
{

SimIni::SimIni():isLoaded_(false)
{

}

SimIni::~SimIni()
{

}

static std::string TrimSpaceLeft(const std::string& str)
{
    if (str.empty()) return str;
    int i = 0;
    for (i=0; i<str.size(); ++i)
    {
        if (isspace(str[i])) continue;
        break;
    }
    if (i == str.size()) return str;
    return str.substr(i);
}

static std::string TrimSpaceRight(const std::string& str)
{
    if (str.empty()) return str;
    int i = 0;
    for (i=0; i<str.size(); ++i)
    {
        char c = str[str.size()-i-1];
        if (isspace(c))
        continue;
        break;
    }
    if (i == str.size()) return str;
    return str.substr(0, str.size()-i);
}

int SimIni::LoadFile(const std::string& file)
{
    ifstream fi(file);
    if (!fi.is_open())
        return 2;
    string line_buf;
    string sess;
    string key;
    string val;
    while (getline(fi, line_buf))
    {
        int i = 0;
        while(line_buf[i]
                && (line_buf[i] == ' ' || line_buf[i] == '\t'
                    || line_buf[i] == '\n'))
        {

            ++i;
        }
        /* parse annotate */
        if (!line_buf[i] || '#' == line_buf[i])
            continue;
        /* parse session */
        if ('[' == line_buf[i])
        {
            sess.clear();
            //sess = "";
            ++i;
            while (line_buf[i])
            {
                if (']' == line_buf[i])
                {
                    break;
                }
                sess += line_buf[i];
                ++i;
            }
            if (!sess.empty())
            {
                key_val_map kvm;
                sessMap_[sess] = kvm;
            }
            continue;
        }
        /* parse key and val */
        if (!sess.empty())
        {

            bool key_flag = false;
            key.clear();
            val.clear();
            while (line_buf[i])
            {
                if ('=' == line_buf[i])
                {
                    key_flag = true;
                    ++i;
                    continue;
                }
                /*deal ' '*/
                if (' ' == line_buf[i])
                {
                    /*ignore the ' ' between key and '='*/
                    if (!key_flag)
                    {
                        ++i;
                        continue;
                    }
                    /* ignore the ' ' between '=' and val */
                    else if (val.empty())
                    {
                        ++i;
                        continue;
                    }
#if 0
                    /* if the space is at the end of val, break */
                    else if (!val.empty())
                    {
                        break;
                    }
#endif
                }
                if (!key_flag)
                    key += line_buf[i];
                else
                    val += line_buf[i];

                ++i;
            }
            if (key_flag && !key.empty())
            {
                key = TrimSpaceLeft(key);
                key = TrimSpaceRight(key);
                val = TrimSpaceLeft(val);
                val = TrimSpaceRight(val);
                sessMap_[sess][key] = val;
            }
        }
        else
        {
            continue;
        }

    }
    fi.close();
    isLoaded_ = true;
    return 0;
}

const std::string emptystr = "";
const std::string& SimIni::GetStringVal(
    const std::string& sess,
    const std::string& key) const
{
    sess_map::const_iterator sit = sessMap_.find(sess);
    if (sit == sessMap_.end())
        return emptystr;
    map<string, string>::const_iterator it = sit->second.find(key);
    if (it == sit->second.end()) return emptystr;
    return it->second;
}

const std::string& SimIni::GetStringValWithDefault(
    const std::string& sess,
    const std::string& key,
    const std::string& defstr) const
{
    const string& str = GetStringVal(sess, key);
    if (str.empty()) return defstr;
    return str;
}

int SimIni::GetIntVal(
    const std::string& sess,
    const std::string& key) const
{
    string s = GetStringVal(sess, key);
    int i = 0;
    if (!s.empty()) i =atoi(s.c_str());
    return i;
}

int SimIni::GetIntValWithDefault(
    const std::string& sess,
    const std::string& key,
    const int defi) const
{
    int i = GetIntVal(sess, key);
    if (0 == i) i = defi;
    return i;
}

std::map<std::string, std::string> SimIni::GetSession(
    const std::string& sess) const
{
    map<string, string> m;
    sess_map::const_iterator sit = sessMap_.find(sess);
    if (sit == sessMap_.end())
        return m;
    m.insert(sit->second.begin(), sit->second.end());
    return m;
}

void SimIni::AddValue(const std::string& sess,
                      const std::string& key,
                      const std::string& val)
{
    sessMap_[sess][key] = val;
}

std::string SimIni::ToString()
{
    string s;
    sess_map::iterator sit;
    key_val_map::iterator kit;
    for (sit=sessMap_.begin(); sit!=sessMap_.end(); ++sit)
    {
        s += "[" + sit->first + "]\n";
        for (kit=sit->second.begin(); kit!=sit->second.end(); ++kit)
            s += kit->first + "=" + kit->second + "\n";
    }
    return s;
}

void SimIni::test()
{
    sess_map::iterator sit;
    key_val_map::iterator kvit;
    for (sit = sessMap_.begin(); sit != sessMap_.end(); ++sit)
    {
        cout<<"["<<sit->first<<"]"<<endl;
        for (kvit = sit->second.begin(); kvit != sit->second.end(); ++kvit)
        {
            cout<<kvit->first<<":"<<kvit->second<<endl;
        }
    }
}

static SimIni s_ini_;
SimIni& SingleSimIni::get()
{
    return s_ini_;
}

}
