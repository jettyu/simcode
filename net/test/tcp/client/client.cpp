#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <simcode/net/Socket.h>
#include <simcode/net/SockAddr.h>
#include <simcode/net/EPollPoller.h>
#include <simcode/net/EventLoop.h>
#include <simcode/net/TcpClient.h>
#include <iostream>
using namespace std;
using namespace simcode;
using namespace simcode::net;

//#include "oci_api.h"

void onConnection(const TcpConnectionPtr& conn)
{
    cout<<"connection success!"<<endl;
    conn->send("conn success", 12);
}

void eventHandle(const TcpConnectionPtr& conn, std::string* msg)
{
    cout<<"recv:"<<*msg<<endl;
    msg->clear();
    sleep(1);
    conn->send("ping", 4);
}

void onClose(const TcpConnectionPtr&)
{
    cout<<"closed ..."<<endl;
}

int main(int argc, char **argv)
{
    EventLoop loop;
    Socket sock(AF_INET, SOCK_STREAM, 0);
    SockAddr peerAddr("127.0.0.1", 8088);
    TcpClient client(&loop, peerAddr, "tcpclient");
    client.setMessageCallback(simex::bind(eventHandle, _1, _2));
    client.setConnectionCallback(simex::bind(onConnection, _1));
    client.setCloseCallback(simex::bind(onClose, _1));
    client.active();
    loop.loop();

    return 0;
}
