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

void eventHandle(const TcpConnectionPtr& conn, const char* data, int size)
{
    //cout<<string(data+2, size-2)<<endl;
    threadQueue.push_back(conn->connfd() , simex::bind(&TcpConnection::send, conn.get(), std::string(data, size)));
   // conn->send(data, size);
}

void codec(const TcpConnectionPtr& conn, std::string *msg)
{
    int length = msg->size();
    int offset = 0;
    while (length > 2)
    {
        if (length < 2) break;
        int size = ntohs(*(uint16_t*)(msg->data()+offset));
        int totle_len = size + 2;
        if (length < totle_len) break;
        eventHandle(conn, msg->data() + offset, totle_len);
        offset += totle_len;
        length -= totle_len;
    }
    if (offset != 0) *msg = msg->substr(msg->size()-length);
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

    return 0;
}
