#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <simcode/net/Socket.h>
#include <simcode/net/SockAddr.h>
#include <simcode/net/EPollPoller.h>
#include <simcode/net/EventLoop.h>
#include <simcode/net/Acceptor.h>
#include <simcode/net/TcpConnection.h>
#include <simcode/net/TcpServer.h>
#include <iostream>
using namespace std;
using namespace simcode;
using namespace simcode::net;

using namespace simcode::thread;

ThreadSafeQueue threadQueue;

void eventHandle(const TcpConnectionPtr& conn, const void* data, int size)
{
    //cout<<string(data+2, size-2)<<endl;
    threadQueue.push_back(conn->connfd() , simex::bind(&TcpConnection::send, conn.get(), std::string((const char*)data, size)));
    //conn->send((const char*)data, size);
}

struct Head
{
    uint16_t size;
    void ParseFromPointer(const void* data)
    {
        *this = *static_cast<const Head*>(data);
        size = ntohs(size);
    }
    void SerializeToPointer(void* data)
    {
        int s = htons(size);
        memcpy(data, this, LENGTH);
    }
    static const size_t LENGTH;
};

const size_t Head::LENGTH = sizeof(Head);

void codec(const TcpConnectionPtr& conn, simcode::net::Buffer *msg)
{
    while (msg->readableBytes() >= Head::LENGTH)
    {
        Head head;
        head.ParseFromPointer(msg->peek());
        int totle_len = head.size + Head::LENGTH;
        if (msg->readableBytes() < totle_len) break;
        //cout<<"msgLen="<<totle_len<<endl;
        eventHandle(conn, msg->peek(), totle_len);
        msg->seek(totle_len);
    }
    msg->retrieve(msg->getSeek());
}

//#include "oci_api.h"

int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);
    threadQueue.setMaxQueueSize(10240);
    threadQueue.setThreadNum(16);
    threadQueue.start();
    EventLoop loop;
    SockAddr listenAddr("0.0.0.0", 10010);
    TcpServer server(&loop, listenAddr, "tcpsvr");
    server.setMessageCallback(simex::bind(codec, _1, _2));
    server.setThreadNum(16);
    server.start();

    loop.loop();
    threadQueue.stop();
    return 0;
}
