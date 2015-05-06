#include <simcode/redis/redis.h>
#include <vector>
using namespace std;
using namespace simcode;
using namespace simcode::redis;

void Redis::swap(Redis& h)
{
    ::swap(m_errcode, h.m_errcode);
    ::swap(m_ctxt, h.m_ctxt);
    m_errmsg.swap(h.m_errmsg);
    m_info.swap(h.m_info);
}

int Redis::Connect(void)
{
    if (m_ctxt)
        return 0;
    if (m_info.host.length() < 5 || m_info.port < 1024)
    {
        char tmp[50] = {0};
        sprintf(tmp, "host:%s|port:%d", m_info.host.c_str(), m_info.port);
        m_errmsg = std::string(tmp);
        return 1;//已经初始化过
    }

    m_errcode = 0;
    m_errmsg.clear();
    m_ctxt = redisConnectWithTimeout(m_info.host.c_str(),
                                     m_info.port,
                                     m_info.time_out);

    if (m_ctxt->err)
    {
        m_errcode = m_ctxt->err;
        m_errmsg = "ctxt:" + std::string(m_ctxt->errstr);
        redisFree(m_ctxt);
        m_ctxt = NULL;
        return 2;
    }
    return 0;
}

redisReply* Redis::Command(const char* format, ...)
{
    va_list argptr;
    va_start(argptr,format);
    redisReply *reply = vCommand(format,argptr);
    va_end(argptr);
    return reply;
}

int Redis::AppendCommand(const char* format, ...)
{
    if (NULL == m_ctxt)
    {
        if (Connect())
            return m_errcode;
    }
    if (m_ctxt->err)
    {
        m_errcode = m_ctxt->err;
        m_errmsg = "ctxt:" + std::string(m_ctxt->errstr);
        return m_errcode;
    }

    va_list argptr;
    va_start(argptr,format);
    int ret = ::redisvAppendCommand(m_ctxt, format, argptr);
    va_end(argptr);

    return ret;
}

redisReply* Redis::GetReply()
{
    if (NULL == m_ctxt)
    {
        return NULL;
    }
    if (m_ctxt->err)
    {
        m_errcode = m_ctxt->err;
        m_errmsg = "ctxt:" + std::string(m_ctxt->errstr);
        return NULL;
    }
    redisReply* r;
    int ret = ::redisGetReply(m_ctxt, (void**)&r);
    if (ret)
    {
        if (m_ctxt->err)
        {
            m_errcode = m_ctxt->err;
            m_errmsg = "ctxt:" + std::string(m_ctxt->errstr);
        }
        if (r)
        {
            m_errcode = r->integer;
            m_errmsg = "reply:" + std::string(r->str);
        }
        return NULL;
    }
    return r;
}

int Redis::GetAllReply(size_t num, std::vector<redisReply* >& rp_vec)
{
    if (NULL == m_ctxt)
        return 1;
    if (m_ctxt->err)
    {
        m_errcode = m_ctxt->err;
        m_errmsg = "ctxt:" + std::string(m_ctxt->errstr);
        return 2;
    }
    for (size_t i=0; i<num; ++i)
    {
        redisReply *r = NULL;
        redisGetReply(m_ctxt, (void **)&r);
        rp_vec.push_back(r);
    }

    if (m_ctxt->err)
    {
        m_errcode=m_ctxt->err;
        m_errmsg="ctxt:"+std::string(m_ctxt->errstr);
    }

    return 0;
}

redisReply* Redis::TryCommand(const char* format, ...)
{
    redisReply* r = NULL;
    va_list argptr;
    va_start(argptr,format);
    r = vTryCommand(format, argptr);
    va_end(argptr);
    return r;

}

redisReply* Redis::vTryCommand(const char* format, va_list argptr)
{
    redisReply* r = NULL;
    for (int i=0; i<2; i++)
    {
        if (r=vCommand(format,argptr), r&&r->type!=REDIS_REPLY_ERROR) break;
        if (0==i) Update();
    }
    return r;
}

redisReply* Redis::TryCommandArgv(const std::vector<std::string>& argvec)
{
    redisReply* r=NULL;
    for (int i=0; i<2; i++)
    {
        if (r=CommandArgv(argvec), r&&r->type!=REDIS_REPLY_ERROR) break;
        if (0==i) Update();
    }
    return r;
}

std::vector<redisReply*>
Redis::TryAppendArgvs(const std::vector<std::vector<std::string> >& argvs)
{
    std::vector<redisReply*> rs;
    for (int i=0; i<2; i++)
    {
        if (rs=AppendArgvs(argvs), !m_errcode) break;
        if (0==i) Update();
    }
    return rs;
}

Redis& Redis::operator=(const Redis& h)
{
    if (this != &h)
    {
        DisConnect();
        m_info.host = h.m_info.host;
        m_info.port = h.m_info.port;
        m_info.time_out = h.m_info.time_out;
    }
    return *this;
}


void Redis::Reset(const std::string& _host,
                  const int _port,
                  const struct timeval& _time_out)
{
    m_info.host = _host;
    m_info.port = _port;
    m_info.time_out = _time_out;
    DisConnect();
}

void Redis::DisConnect()
{
    if (m_ctxt)
        redisFree(m_ctxt);
    m_ctxt = NULL;
    m_errcode = 0;
    m_errmsg.clear();
}

redisReply* Redis::vCommand(const char* format, va_list argptr)
{
    //如果没有连接或连接错误 并且重连失败，则直接返回
    if ((!m_ctxt || m_ctxt->err) && Update()) return NULL;
    return (redisReply*)redisvCommand(m_ctxt, format, argptr);
}

redisReply* Redis::CommandArgv(const std::vector<std::string>& argvec)
{
    if ((!m_ctxt || m_ctxt->err) && Update()) return NULL;
    int argc = argvec.size();
    vector<const char *> argv;
    vector<size_t> argvlen;
    argv.reserve(argc);
    argvlen.reserve(argc);
    vector<string>::const_iterator it;
    for (it=argvec.begin(); it!=argvec.end(); ++it)
    {
        argv.push_back(it->c_str());
        argvlen.push_back(it->length());
    }
    return (redisReply *)redisCommandArgv(m_ctxt,
                                          argc, argv.data(), argvlen.data());
}

int Redis::AppendCommandArgv(const std::vector<std::string>& argvec)
{
    if ((!m_ctxt || m_ctxt->err) && Update()) return 0;
    int argc = argvec.size();
    vector<const char *> argv;
    vector<size_t> argvlen;
    argv.reserve(argc);
    argvlen.reserve(argc);
    vector<string>::const_iterator it;
    for (it=argvec.begin(); it!=argvec.end(); ++it)
    {
        argv.push_back(it->c_str());
        argvlen.push_back(it->length());
    }
    return redisAppendCommandArgv(m_ctxt, argc, argv.data(), argvlen.data());
}

std::vector<redisReply*>
Redis::AppendArgvs(const std::vector<std::vector<std::string> >& argvs)
{
    vector<redisReply*> rs;
    if ((!m_ctxt || m_ctxt->err) && Update()) return rs;
    vector<vector<string> >::const_iterator it;
    for (it=argvs.begin(); it!=argvs.end(); ++it)
    {
        int ret = 0;
        if(ret=AppendCommandArgv(*it), ret)
        {
            if (m_ctxt->err)
            {
                m_errcode = m_ctxt->err;
                m_errmsg = "ctxt:" + std::string(m_ctxt->errstr);
                continue;
            }
            m_errcode = ret;
            m_errmsg = "append failed";
        }
    }
    for (size_t j=0; j<argvs.size(); ++j)
    {
        int ret = 0;
        redisReply *r = NULL;
        if(ret=redisGetReply(m_ctxt, (void **)&r), ret)
        {
            if (m_ctxt->err)
            {
                m_errcode = m_ctxt->err;
                m_errmsg = "ctxt:" + std::string(m_ctxt->errstr);
                continue;
            }
            m_errcode = ret;
            m_errmsg = "getreply failed";
        }
        rs.push_back(r);
    }
    return rs;
}

