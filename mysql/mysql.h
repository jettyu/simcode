#ifndef SIMSQL_H
#define SIMSQL_H

#include <string>
#include <vector>
#include <mysql/mysql.h>

namespace simcode
{

namespace mysql
{

struct MysqlInfo
{
    std::string host;
    int port;
    std::string user;
    std::string passwd;
    std::string db;
    bool operator < (const MysqlInfo& info) const
    {
        if (host != info.host) return host < info.host;
        else return port < info.port;
    }
};

struct MysqlOption
{
    mysql_option option;
    std::string arg;
};

typedef std::vector<MysqlOption> MysqlOptionVec;

class Mysql
{
public:
    int errcode()
    {
        return mysql_errno(sql_);
    }
    std::string errmsg()
    {
        return mysql_error(sql_);
    }
    Mysql():sql_(NULL),status_(0) {}
    Mysql(const MysqlInfo& in):sql_(NULL),status_(0)
    {
        Reset(in);
    }
    ~Mysql()
    {
        Destroy();
    }
    void Init(void)
    {
        if(!sql_) sql_=mysql_init(sql_);
    };
    void Destroy(void);
    int Options(mysql_option opt, const std::string& arg);
    int Connect(void);
    void DisConnect(void);
    int SetConnectUtf8();
    int SetCharSet(const std::string& charset="utf8");
    int Update(void);
    void Reset(const MysqlInfo& _info)
    {
        DisConnect();
        info_=_info;
    }
    int Query(const std::string& sql);
    MYSQL_RES* QueryRes(const std::string& sql);
    bool Ping();
    long InsertId(void)
    {
        return mysql_insert_id(sql_);
    }
    std::string RealEscape(const std::string& str);
    static std::string Escape(const std::string& str);

    int TryQuery(const std::string& sql);
    MYSQL_RES* TryQueryRes(const std::string& sql);
private:
    Mysql( const Mysql& ) {}
    Mysql& operator=( const Mysql& )
    {
        return *this;
    }
private:
    MYSQL *sql_;
    //status_ bitset 0,1,2,3,4,5,6,7
    //0-conn 1-setutf8
    uint8_t status_;
    MysqlInfo info_;
    MysqlOptionVec options_;
    std::string charset_;
};

inline void Mysql::Destroy()
{
    DisConnect();
    sql_ = NULL;
    status_ = 0;
    options_.clear();
}


#define CHECK_MYSQL_ERROR_RET(ptr, err_ret) do{\
    if (ptr->errcode()){\
        LOG_APP_ERROR("mysql error|errcode:%d|errmsg:%s",\
            ptr->errcode(), ptr->errmsg().c_str());\
        return err_ret;\
    }}while(0)
}//endof namespace mysql

}//endof namespace sim

#endif
