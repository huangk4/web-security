
#include "stdafx.h"


/*
������	CTCP
���ܣ�	ʵ��TCPͨ��
*/

/*
���캯��
���ܣ�	��ʼ��SOCKETͨ�ſ⣬ҪȥWINSOCK2
������	lowver:�汾�ĵ�λ
higver:�汾�ĸ�λ
����ֵ��TRUE��ʾ�ɹ��ܣ�FALSEΪʧ��
*/
CTCP::CTCP(int lowver, int higver)
{
	m_Socket = INVALID_SOCKET;
	WORD wVersion = 0;
	int err = -1;
	WSADATA wsaData;

	//���ֽ�Ϊ���汾�����ֽ�Ϊ���汾
	wVersion = MAKEWORD(lowver, higver);
	err = WSAStartup(wVersion, &wsaData);

	if (LOBYTE(wsaData.wHighVersion) < 2 || HIBYTE(wsaData.wVersion) < 2)
	{
		MessageBox(NULL, L"winsocket��汾��", L"����ʧ��", MB_OK);
	}
}

//��������
CTCP::~CTCP()
{

}



/*
���ܣ�	�������ͳ�ʼ��SOCKET��Դ
������	SocketType:SOCK_BIND �󶨱��ض˿ڣ�SOCK_NOBIND ����
		BindIp:Ҫ�󶨵�IP��ַ,""Ϊ���������ַ�����ʮ���Ʊ�ʾIP��ַ
		BindPort:Ҫ�󶨵ı��ض˿ڣ�Ϊ0��ϵͳ�Զ�����
		opt�������Ƿ�֧�ֶ˿����ã���Ϊ��չ���ܣ���δʵ��
����ֵ��
		����INVALID_SOCKET
		��ȷ�����ؿ�����SOCKET
*/
SOCKET CTCP::InitSocket(int SocketType, string strBindIp, u_short BindPort, int opt)
{
	SOCKET socketid = INVALID_SOCKET;
	socketid = socket(PF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN sockStruct;
	//ʹ��TCP/IPЭ��
	sockStruct.sin_family = AF_INET;
	//��IP
	if (strBindIp.empty())
	{
		sockStruct.sin_addr.S_un.S_addr = INADDR_ANY;
	}
	else
	{
		sockStruct.sin_addr.S_un.S_addr = inet_addr(strBindIp.c_str());
	}
	
	sockStruct.sin_port = htons(BindPort);
	if (SocketType == SOCK_NOBIND)
	{
		if (connect(socketid, (LPSOCKADDR)&sockStruct, sizeof(sockStruct)) == SOCKET_ERROR)
		{
			//����ָ��IP�����ʧ��
			closesocket(socketid);
			shutdown(socketid, 2);
			socketid = INVALID_SOCKET;
		}
		m_Socket = socketid;
	}
	else if (SocketType == SOCK_BIND)
	{
		if (bind(socketid, (sockaddr*)&sockStruct, sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			//�󶨶˿�ʧ��
			closesocket(socketid);
			socketid = INVALID_SOCKET;
		}
		else
		{
			if (listen(socketid, SOMAXCONN) == SOCKET_ERROR)
			{
				//������������ʧ��
				closesocket(socketid);
				socketid = INVALID_SOCKET;
			}
		}
		m_Socket = socketid;
	}
	return socketid;
}

/*
���ܣ�	����һ���µ��׽��������addrָ���Ŀͻ����׽��ֽ�������
������	s�����ڼ���״̬�����׽���
		addr���´������׽��ֵ�ַ�ṹ
		addrlen���´����׽��ֵ�ַ�ṹ�ĳ���
����ֵ������ʧ�ܣ�INVALID_SOCKET
		���óɹ������õ�SOCKET
*/
SOCKET CTCP::myaccept(SOCKET s, struct sockaddr* addr, int* addrlen)
{
	SOCKET acceptsocket = INVALID_SOCKET;
	acceptsocket = accept(s, addr, addrlen);
	return acceptsocket;
}

/*
���ܣ�	�������ͳ�ʼ��SOCKET��Դ
������	sock�����ն��׽���������
		buf��������Ž��յ������ݵĻ�����
		len�����յ������ݵĴ�С
		flag��Ĭ������
		overtime����ʱʱ�䣬��Ϊ��λ
		EndMark���������
		Soonflag���Ƿ���������
����ֵ�����յ������ݵ��ֽ���
*/
int CTCP::myrecv(SOCKET sock, char *buf, int len, int flag, UINT overtime,const char *EndMark, BOOL soonflag)
{
	int ret;
	int nLeft = len;
	int idx = 0;
	int nCount = 0;
	fd_set readfds;		//�ļ��������ļ���
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 500;
	DWORD s_time = GetTickCount();	//����ϵͳ���������ڵĺ�����
	while (nLeft > 0)
	{
		MSG msg;
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (msg.message == WM_QUIT)
			return 0;
		FD_ZERO(&readfds);//��set����
		FD_SET(sock, &readfds);//��fd����set

		if (select(0, &readfds, NULL, NULL, &timeout) == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		DWORD e_time = GetTickCount();
		if (!FD_ISSET(sock, &readfds))
		{
			if (e_time - s_time > overtime * 1000)	//��ʱ
				return SOCKET_TIMEOUT;
			else
				continue;
		}
		ret = recv(sock, &buf[idx], nLeft, flag);
		int b=WSAGetLastError();
		if (soonflag == TRUE)
		{
			return ret;
		}
		s_time = e_time;
		if (ret <= 0)
		{
			int LastError = GetLastError();
			if ((-1 == ret) && (WSAETIMEDOUT == LastError))
				continue;
			if ((-1 == ret) && (WSAEWOULDBLOCK == LastError))
			{
				if (nCount < 2000)
				{
					Sleep(10);
					nCount++;
					continue;
				}
			}
			return ret;
		}
		nCount = 0;
		nLeft -= ret;
		idx += ret;
		if (EndMark != NULL&&idx > 5)
		{
			if (strstr(buf + (idx - 5), EndMark) != NULL)
			{
				break;
			}
		}
	}
	return idx;
}

/*
���ܣ�	��ָ����Socket��������
������	socket:���Ͷ��׽���������
		buf���������Ҫ�������ݵĻ�����
		flag��һ������
		overtime�����ó�ʱʱ��
����ֵ��ʵ�ʷ������ݵ��ֽ���
*/

int CTCP::mysend(SOCKET sock, const char *buf, int len, int flag, UINT overtime)
{
	int ret;
	int nLeft = len;
	int idx = 0;

	fd_set readfds;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 500;
	DWORD s_time = GetTickCount();

	while (nLeft > 0)
	{
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);

		int errorret = select(0, NULL, &readfds, NULL, &timeout);
		if (errorret == SOCKET_ERROR)
		{
			OutputDebugString(L"mysend  socket����");
			return SOCKET_ERROR;
		}
		DWORD e_time = GetTickCount();
		if (!FD_ISSET(sock, &readfds))
		{
			if (e_time - s_time > overtime * 1000)	//��ʱ
			{
				OutputDebugString(L"mysend �������ݳ�ʱ");
				return 0;
			}
			else
			{
				continue;
			}
		}
		ret = send(sock, &buf[idx], nLeft, flag);

		if (ret <= 0)
			return ret;
		nLeft -= ret;
		idx += ret;
	}
	return len;
}