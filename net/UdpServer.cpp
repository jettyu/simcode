#include <simcode/net/UdpServer.h>
#include <simcode/net/EventChannel.h>
#include <simcode/base/logger.h>
using namespace simcode;
using namespace net;
using namespace thread;

UdpServer::UdpServer(EventLoop* loop, const SockAddr& addr, const std::string& name):
    loop_(loop),
    socket_(PF_INET, SOCK_DGRAM, 0),
    threadNum_(0),
    sendLoop_(loop)
{
    assert(socket_.sockfd() != -1);
    socket_.setReuseAddr();
    socket_.setReusePort();
    socket_.setNonBlockAndCloseOnExec();
    assert(socket_.Bind(addr) != -1);
    channel_.reset(new EventChannel(loop, socket_.sockfd(), simex::bind(&UdpServer::eventHandle, this, _1)));
    channel_->enableReading();
}

void UdpServer::start()
{
    queue_.setThreadNum(threadNum_+1);
    queue_.start();
    connectionManager_.reset(new UdpConnectionManager(sendLoop_, simex::bind(&UdpServer::send, this, _1,_2)));
    loop_->runInLoop(channel_);
}

void UdpServer::setThreadNum(int n)
{
    threadNum_ = n;
}

void UdpServer::eventHandle(EventChannel*)
{
    queue_.push_back(threadNum_+1, SimBind(&UdpServer::hanleRead, this));
}

void UdpServer::hanleRead()
{
    connectionManager_->AddTask(simex::bind(&UdpServer::onMessage, this, _1));
}

void UdpServer::onMessage(const simex::any& connStore)
{
    std::map<uint64_t, UdpConnectionPtr>* connMap = simex::any_cast<std::map<uint64_t, UdpConnectionPtr>*>(connStore);
    std::map<uint64_t, UdpConnectionPtr>::iterator it;
    int n = 0;
    do
    {
        UdpConnector c(socket_.sockfd());
        char tmp[65535];
        if ((n=c.recv(tmp, 65535)) < 0)
        {
            LOG_DEBUG("read error|errno=%d|errmsg=%s", errno, strerror(errno));
            return;
        }
        UdpConnectionPtr conn;
        uint64_t id = c.peerAddr().id();
        it = connMap->find(id);
        if (it == connMap->end())
        {
            LOG_DEBUG("recv new client|ip=%s|port=%u|id=%lu",
                      c.peerAddr().ip().c_str(), c.peerAddr().port(), id);
            conn.reset(new UdpConnection(c));
            conn->setCloseCallback(SimBind(&UdpServer::onClose, this, _1));
            connectionManager_->Add(id, conn);
        }
        if (threadNum_)
            queue_.push_back(conn->id()%threadNum_, simex::bind(&UdpServer::asyncMessage, this, conn, std::string(tmp, n)));
        else
            messageCallback_(conn, tmp, n);
    }
    while (n > 0);
}

void UdpServer::onClose(uint64_t connId)
{
    LOG_DEBUG("client remove|id=%ld", connId);
    connectionManager_->Remove(connId);
}

void UdpServer::send(const UdpConnectionPtr& conn, const std::string& data)
{
    conn->sendString(data);
}

void UdpServer::asyncMessage(const UdpConnectionPtr& conn, const std::string& data)
{
    messageCallback_(conn, data.data(), data.size());
}

