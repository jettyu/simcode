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

void Recv(const UdpConnectionPtr& conn, std::string* msg)
{
    cout<<"recv:"<<*msg<<endl;
    sleep(1);
    conn->Send("ping", 4);
}

int main(int argc, char**argv)
{
    EventLoop loop;
    SockAddr addr("127.0.0.1", 8088);
    UdpClient client(&loop, addr, "client");
    client.setMessageCallback(simex::bind(Recv, _1, _2));
    if (-1 == client.Send("ping", 4))
    {
        cout<<strerror(errno)<<endl;
    }
    loop.loop();
    return 0;
}
