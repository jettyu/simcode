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


void eventHandle(const UdpConnectionPtr& conn, std::string* msg)
{
    cout<<"recv:"<<*msg<<endl;
    conn->Send("pong", 4);
}

int main(int argc, char **argv)
{
    EventLoop loop;
    UdpServer server(&loop, SockAddr("0.0.0.0", 8088), "server");
    server.setThreadNum(1);
    server.setMessageCallback(SimBind(eventHandle,_1,_2));
    server.start();
    loop.loop();

    return 0;
}
