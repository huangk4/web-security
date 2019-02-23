#pragma once

#include "winsock2.h"


#define SOCK_BIND 1	//监听本地
#define SOCK_NOBIND 2	//主动连接
#define SOCKET_TIMEOUT -100	//套接字超时

class CTCP
{
private:
	SOCKET m_Socket;
public:
	CTCP(int lowver, int higver);
	virtual ~CTCP();
public:

	//初始化SOCKET函数
	SOCKET InitSocket(int SocketType, string strBindIp, u_short BindPort, int opt=0);

	//处理本地监听函数
	SOCKET myaccept(SOCKET s, struct sockaddr* addr, int* addrlen);

	//向目标发送数据函数
	int mysend(SOCKET sock, const char *buf, int len, int flag=0, UINT overtime=1);

	//接收数据
	int myrecv(SOCKET sock, char *buf, int len, int flag = 0, UINT overtime = 5, const char *EndMark = "\0", BOOL soonflag = FALSE);
};