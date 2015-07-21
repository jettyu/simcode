#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <simcode/net/Socket.h>
#include <simcode/net/SockAddr.h>
#include <simcode/net/EventLoopThread.h>
#include <simcode/net/TcpClient.h>
#include <iostream>
using namespace std;
using namespace simcode;
using namespace simcode::net;

//#include "oci_api.h"

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

void eventHandle(const TcpConnectionPtr& conn, const void* msg, int size)
{
    cout<<"recv:"<<std::string((const char*)msg+Head::LENGTH, size - Head::LENGTH)<<endl;
}

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

void onConnection(const TcpConnectionPtr& conn)
{
    cout<<"connection success!"<<endl;
    //conn->send("conn success", 12);
}

void onClose(const TcpConnectionPtr&)
{
    cout<<"closed ..."<<endl;
}

int main(int argc, char **argv)
{
    EventLoopThread loop;
    Socket sock(AF_INET, SOCK_STREAM, 0);
    SockAddr peerAddr("127.0.0.1", 10010);
    TcpClient client(loop.startLoop(), peerAddr, "tcpclient");
    client.setMessageCallback(simex::bind(codec, _1, _2));
    client.setConnectionCallback(simex::bind(onConnection, _1));
    client.setCloseCallback(simex::bind(onClose, _1));
    client.active();
    std::string inputStr;
    std::string sendStr;
    while (1)
    {
        inputStr.clear();
        cin >> inputStr;
        if (inputStr == "exit") return 0;
        Head head;
        head.size = inputStr.size();
        sendStr.resize(Head::LENGTH);
        head.SerializeToPointer(&sendStr[0]);
        sendStr.append(inputStr);
        client.sendString(sendStr);
    }
    return 0;
}
