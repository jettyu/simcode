#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <simcode/net/Socket.h>
#include <simcode/net/SockAddr.h>
#include <simcode/net/EventLoop.h>
#include <simcode/net/TcpServer.h>
#include <iostream>
using namespace std;
using namespace simcode;
using namespace simcode::net;

void eventHandle(const TcpConnectionPtr& conn, const char* data, int size)
{
    cout<<string(data+2, size-2)<<endl;
    conn->send(data, size);
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
        size = htons(size);
        memcpy(data, this, LENGTH);
    }
    static const size_t LENGTH;
};

const size_t Head::LENGTH = sizeof(Head);

void codec(const TcpConnectionPtr& conn, simcode::Buffer *msg)
{
    while (msg->readableBytes() >= Head::LENGTH)
    {
        Head head;
        head.ParseFromPointer(msg->peek());
        int totle_len = head.size + Head::LENGTH;
        cout<<"msgLen="<<totle_len<<endl;
        if (msg->readableBytes() < totle_len) break;

        eventHandle(conn, (const char*)msg->peek(), totle_len);
        msg->seek(totle_len);
    }
    msg->retrieve(msg->getSeek());
}

//#include "oci_api.h"

int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);
    EventLoop loop;
    SockAddr listenAddr("0.0.0.0", 10010);
    TcpServer server(&loop, listenAddr, "tcpsvr");
    server.setMessageCallback(simex::bind(codec, _1, _2));
    server.start();
    loop.loop();
    return 0;
}
