#pragma once

#include "winsock2.h"


#define SOCK_BIND 1	//��������
#define SOCK_NOBIND 2	//��������
#define SOCKET_TIMEOUT -100	//�׽��ֳ�ʱ

class CTCP
{
private:
	SOCKET m_Socket;
public:
	CTCP(int lowver, int higver);
	virtual ~CTCP();
public:

	//��ʼ��SOCKET����
	SOCKET InitSocket(int SocketType, string strBindIp, u_short BindPort, int opt=0);

	//�����ؼ�������
	SOCKET myaccept(SOCKET s, struct sockaddr* addr, int* addrlen);

	//��Ŀ�귢�����ݺ���
	int mysend(SOCKET sock, const char *buf, int len, int flag=0, UINT overtime=1);

	//��������
	int myrecv(SOCKET sock, char *buf, int len, int flag = 0, UINT overtime = 5, const char *EndMark = "\0", BOOL soonflag = FALSE);
};