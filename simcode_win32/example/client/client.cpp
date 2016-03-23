#include <simcode/net/TcpClient.h>
#pragma  comment(lib,"..\\win32\\lib\\debug\\win32.lib")
#pragma  comment(lib,"ws2_32.lib")

using namespace std;
using namespace simcode;
using namespace net;
static void onMessage(const TcpConnection::Ptr& conn, Buffer* msg)
{
    while(msg->readableBytes() > 2)
    {
        size_t size = ntohs(*static_cast<const u_short*>(msg->peek()));
        if (msg->readableBytes() < size) break;
        printf("recv:%s\n", (const char*)msg->peek()+2);
        msg->seek(size+2);
		Sleep(1000);

		{
			std::string sendData = "00hello!\n";
			unsigned short size = htons(sendData.size()-2);
			memcpy(&sendData[0], &size, sizeof(size));
			conn->sendString(sendData);
		}

    }
    msg->retrieveSeek();
}

static void onConnect(const TcpConnection::Ptr& conn)
{
	std::string sendData = "00hello!\n";
	unsigned short size = htons(sendData.size()-2);
	memcpy(&sendData[0], &size, sizeof(size));
	conn->sendString(sendData);
}

static void onError(const TcpConnection::Ptr& conn)
{
	printf("errcode=%d\n", WSAGetLastError());
}

int main(int argc, char* argv[])
{
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA data;
    if(WSAStartup(sockVersion, &data) != 0)
    {
        return 0;
    }
	std::string sendData = "00hello!\n";
    unsigned short size = htons(sendData.size()-2);
    memcpy(&sendData[0], &size, sizeof(size));

	EventLoop loop;
	SockAddr svrAddr("127.0.0.1", 10010);

	TcpClient client(&loop,svrAddr, "echo_client");
	client.setConnectionCallback(simex::bind(onConnect, _1));
	client.setMessageCallback(simex::bind(onMessage, _1, _2));
	client.setErrorCallback(simex::bind(onError, _1));
	client.setRetry(true);
	client.active();
	Sleep(1000);
	client.sendString(sendData);

    loop.loop();

    WSACleanup();
    return 0;
}