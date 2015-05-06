#include <simcode/base/so_handler.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <iostream>
using namespace std;

namespace simcode
{

int SoLoad::FuncLoad(const std::string& sofile,
                     const std::vector<std::string>& symbols,
                     std::map<std::string, std::vector<void*>>& sofuncs)
{
    void* handle = dlopen(sofile.c_str(), RTLD_NOW);
    if (!handle)
    {
        errcode_ = errno;
        errmsg_ = dlerror();
        cout<<__FILE__<<__LINE__<<"|"<<errmsg_<<endl;
        return errcode_;
    }
    handlers_.push_back(handle);
    dlerror();
    vector<string>::const_iterator it;
    for (it=symbols.begin(); it!=symbols.end(); ++it)
    {
        void* sofunc = dlsym(handle, it->c_str());
        if (!sofunc)
        {
            errcode_ = errno;
            errmsg_ = dlerror();
            cout<<__FILE__<<__LINE__<<"|"<<errmsg_<<endl;
            return errcode_;
        }
        sofuncs[*it].push_back(sofunc);
    }
    return 0;
}

int SoLoad::FuncLoadFromPath(const std::string& path,
                             const std::vector<std::string>& symbols,
                             std::map<std::string, std::vector<void*>>& sofuncs)
{
    struct dirent       **namelist;
    int n = scandir(path.c_str(), &namelist, NULL, alphasort);
    if (n < 0)
    {
        errcode_ = errno;
        return errcode_;
    }
    string sofilename;
    while (n--)
    {
        struct dirent *ent = namelist[n];
        if (!(ent->d_type & DT_DIR))
        {
            //LOG_ERROR("%s|%s|%d\n", __FILE__, __FUNCTION__, __LINE__);
            //LOG_ERROR("name=%s|type=%d\n", ent->d_name, ent->d_type);
            size_t filename_len = strlen(ent->d_name);
            if (filename_len > 3
                    && !strncmp(ent->d_name+filename_len-3, ".so", 3))
            {
                sofilename = path + string("/") + ent->d_name;
                FuncLoad(sofilename.c_str(), symbols, sofuncs);
            }
        }
        free(ent);
    }
    free(namelist);
    return 0;
}

void SoLoad::Close(void)
{
    vector<void*>::iterator it;
    for (it=handlers_.begin(); it!=handlers_.end(); ++it)
        dlclose(*it);
    handlers_.clear();
}

}
