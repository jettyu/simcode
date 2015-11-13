#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <simcode/net/UdpClient.h>
#include <iostream>
using namespace std;
using namespace simcode;
using namespace simcode::net;

void Recv(const UdpConnectionPtr& conn, const char* buf, size_t size)
{
    cout<<"recv:"<<buf<<endl;
    sleep(1);
    conn->send("ping", 4);
}

int main(int argc, char**argv)
{
    EventLoop loop;
    SockAddr addr("192.168.1.201", 10080);
    UdpClient client(&loop, addr, "client");
    client.setMessageCallback(simex::bind(Recv, _1, _2, _3));
    
    while(1)
    {
    if (-1 == client.send("ping", 4))
    {
        cout<<strerror(errno)<<endl;
    }
    sleep(1);
    }
    loop.loop();
    return 0;
}
