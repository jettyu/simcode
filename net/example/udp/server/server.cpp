#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <simcode/net/Socket.h>
#include <simcode/net/SockAddr.h>
#include <simcode/net/UdpConnection.h>
#include <simcode/net/UdpServer.h>
#include <iostream>
using namespace std;
using namespace simcode;
using namespace simcode::net;

//#include "oci_api.h"


void eventHandle(const UdpConnectionPtr& conn, const char*buf, size_t size)
{
    cout<<"recv:"<<buf<<endl;
    conn->send("pong", 4);
}

int main(int argc, char **argv)
{
    EventLoop loop;
    UdpServer server(&loop, SockAddr("0.0.0.0", 10010), "server");
    server.setMessageCallback(simex::bind(eventHandle,_1,_2,_3));
    server.setThreadNum(1);
    server.start();
    loop.loop();

    return 0;
}
