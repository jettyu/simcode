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

//#include "oci_api.h"
void eventHandle(const TcpConnectionPtr& conn, std::string* msg)
{
    cout << "recv:" << *msg <<endl;
    conn->send(msg->data(), msg->size());
    msg->clear();
}

int main(int argc, char **argv)
{
    EventLoop loop;
    SockAddr listenAddr("0.0.0.0", 8088);
    TcpServer server(&loop, listenAddr, "tcpsvr");
    server.setMessageCallback(simex::bind(eventHandle, _1, _2));
    server.start();
    loop.loop();

    return 0;
}
