#include <simcode/mysql/mysql.h>
#include <simcode/base/typedef.h>
using namespace simcode;
using namespace simcode::mysql;

static Mutex init_mtx;

int Mysql::Options(mysql_option option, const std::string& arg)
{
    options_.push_back({option, arg});
    return 0;
}

void Mysql::Init(void)
{
    if(!sql_) 
    {
        ScopeLock lock(init_mtx);
        sql_=mysql_init(sql_);
    }
}

int Mysql::Connect()
{
    Init();
    for (MysqlOptionVec::iterator it=options_.begin(); it!=options_.end(); ++it)
    {
        if (mysql_options(sql_, it->option, it->arg.c_str()))
            return 1;
    }
    if (!mysql_real_connect(sql_,
                            info_.host.c_str(),
                            info_.user.c_str(),
                            info_.passwd.c_str(),
                            info_.db.c_str(),
                            info_.port,
                            NULL,
                            info_.clientflag)
       )
    {
        return 1;
    }
    char value = 1;
    mysql_options(sql_, MYSQL_OPT_RECONNECT, (char *)&value); //设置Ping时自动重连
//    SetConnectUtf8();
    status_ |= 1;
    return 0;
}

void Mysql::DisConnect()
{
    if ((status_ & 1) && sql_) 
    {
        mysql_close(sql_);
    }
    status_ &= ~(1);
    sql_ = NULL;
}

int Mysql::SetConnectUtf8()
{
    status_ |= 2;
    const char *sql = "SET character_set_connection='utf8', character_set_results='utf8', character_set_client='utf8'";
    return Query(sql);
}

int Mysql::SetCharSet(const std::string& charset)
{
    Init();
    if (charset_.empty()) charset_ = charset;
    return mysql_set_character_set(sql_, charset.c_str());
}

int Mysql::Update()
{
    DisConnect();
    if (Connect()) return 1;
    if (status_ & 2) SetConnectUtf8();
    if (!charset_.empty()) SetCharSet(charset_);
    return 0;
}

int Mysql::Query(const std::string& sql)
{
    //if (!(status_&1) && Update()) return 1;
    return mysql_real_query(sql_, sql.data(), sql.size());
}

MYSQL_RES* Mysql::QueryRes(const std::string& sql)
{
    if (Query(sql))    return NULL;
    return mysql_store_result(sql_);
}

bool Mysql::Ping()
{
    if ((status_&1) && !mysql_ping(sql_))
        return true;
    return false;
}

std::string Mysql::Escape(const std::string& str)
{
    std::string to;
    to.resize(str.length()*2 + 1);
    size_t len = mysql_escape_string(&to[0], str.c_str(), str.length());
    to.resize(len);
    return to;
}
std::string Mysql::RealEscape(const std::string& str)
{
    std::string to;
    to.resize(str.length()*2 + 1);
    size_t len = mysql_real_escape_string(sql_, &to[0], str.c_str(), str.length());
    to.resize(len);
    return to;
}

int Mysql::TryQuery(const std::string& sql)
{
    if (!Query(sql)) return 0;
    if (Update()) return errcode();
    return Query(sql);
}

MYSQL_RES* Mysql::TryQueryRes(const std::string& sql)
{
    MYSQL_RES* res = QueryRes(sql);
    if (!res && !Update()) res = QueryRes(sql);
    return res;
}


