#include <winsock2.h>  
#include <WS2tcpip.h>
#include<iostream>
#include<stdio.h>
using namespace std;
#pragma comment(lib, "ws2_32.lib")
bool send_data(int fd, const char *data, int len)
{
	int sl=0;
	while (sl<len)
	{
		int s = send(fd,data+sl,len-sl,0);
		if(s<=0)
		{
			printf("send error:%d\n",s);
			return false;
		}
		sl+=s;
	}
	return true;
}
char BUF[1024];
int main()
{
    printf("I'm test_target\n");
    	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(80);
	inet_pton(AF_INET, "220.181.57.216", &serAddr.sin_addr);
    	while (true) 
	{
		SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sclient == INVALID_SOCKET)
		{
			printf("invalid socket!\n");
			break;
		}
		if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{  
			printf("connect error !\n");
			closesocket(sclient);
			break;
		}
		printf("connect success\n");
		{
			std::string sd;
			sd.append("GET / HTTP/1.1\r\n");
			sd.append("Host: www.baidu.com\r\n");
			sd.append("Connection: close\r\n\n");//close connection after get result
			
			if (!send_data(sclient, sd.c_str(), sd.size()))
			{
				printf("send data error\n");
				break;
			}

			while(true)
			{
				int ret = recv(sclient, BUF, sizeof(BUF), 0);
				if (ret == 0)
				{
					printf("finish receiving connect closed by peer\n");
					break;
				}
				else if (ret < 0)
				{
					printf("something error");
					break;
				}
				else
				{
					for(int i=0;i<ret;i++)
					{
						printf("%c",BUF[i]);
					}
				}
			}
		}
		closesocket(sclient);
		Sleep(1000);
	}
	WSACleanup();
//	system("pause");

    return 0;
}