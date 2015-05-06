#ifndef SO_HANDLER_H
#define SO_HANDLER_H

#include <vector>
#include <string>
#include <map>

namespace simcode
{
typedef std::map<std::string, std::vector<void*>> SoFuncsMap;
class SoLoad
{
public:
    SoLoad():errcode_(0) {}
    ~SoLoad()
    {
        Close();
    }
    int FuncLoad(const std::string& sofile,
                 const std::vector<std::string>& symbols,
                 SoFuncsMap& sofuncs);
    int FuncLoadFromPath(const std::string& path,
                         const std::vector<std::string>& symbols,
                         SoFuncsMap& sofuncs);
    void Close(void);
    inline const int errcode() const
    {
        return errcode_;
    }
    inline const std::string& errmsg() const
    {
        return errmsg_;
    }
private:
    int errcode_;
    std::string errmsg_;
    std::vector<void*> handlers_;
};

}

#endif
