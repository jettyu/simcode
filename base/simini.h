#ifndef _SIMINI_H
#define _SIMINI_H

#include <map>
#include <string>
namespace simcode
{
class SimIni
{
public:
    /*key-val*/
    typedef std::map<std::string, std::string> key_val_map;
    /*session - (key-val)*/
    typedef std::map<std::string, key_val_map> sess_map;
    SimIni();
    ~SimIni();
    inline bool is_loaded()
    {
        return isLoaded_;
    }
    inline void clear()
    {
        sessMap_.clear();
    }
    int LoadFile(const std::string& file);
    const std::string& GetStringVal(
        const std::string& sess,
        const std::string& key) const;
    const std::string& GetStringValWithDefault(
        const std::string& sess,
        const std::string& key,
        const std::string& defstr) const;
    int GetIntVal(
        const std::string& sess,
        const std::string& key) const;
    int GetIntValWithDefault(
        const std::string& sess,
        const std::string& key,
        const int defi) const;

    std::map<std::string, std::string> GetSession(
        const std::string& sess) const;
    const sess_map& GetAllSession() const
    {
        return sessMap_;
    }

    void AddValue(const std::string& sess,
                  const std::string& key,
                  const std::string& val);
    std::string ToString();
public:
    void test();
protected:
private:
    bool isLoaded_;
    sess_map sessMap_;
};

class SingleSimIni
{
public:
    static SimIni& get();
};

}

#endif
