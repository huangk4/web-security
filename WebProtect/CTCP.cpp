
#include "stdafx.h"


/*
类名：	CTCP
功能：	实现TCP通信
*/

/*
构造函数
功能：	初始化SOCKET通信库，要去WINSOCK2
参数：	lowver:版本的低位
higver:版本的高位
返回值：TRUE表示成功能，FALSE为失败
*/
CTCP::CTCP(int lowver, int higver)
{
	m_Socket = INVALID_SOCKET;
	WORD wVersion = 0;
	int err = -1;
	WSADATA wsaData;

	//低字节为主版本，高字节为副版本
	wVersion = MAKEWORD(lowver, higver);
	err = WSAStartup(wVersion, &wsaData);

	if (LOBYTE(wsaData.wHighVersion) < 2 || HIBYTE(wsaData.wVersion) < 2)
	{
		MessageBox(NULL, L"winsocket库版本低", L"运行失败", MB_OK);
	}
}

//析构函数
CTCP::~CTCP()
{

}



/*
功能：	根据类型初始化SOCKET资源
参数：	SocketType:SOCK_BIND 绑定本地端口，SOCK_NOBIND 不绑定
		BindIp:要绑定的IP地址,""为本地任意地址，点分十进制表示IP地址
		BindPort:要绑定的本地端口；为0则系统自动产生
		opt：用于是否支持端口重用，作为扩展功能，暂未实现
返回值：
		错误：INVALID_SOCKET
		正确：返回可以用SOCKET
*/
SOCKET CTCP::InitSocket(int SocketType, string strBindIp, u_short BindPort, int opt)
{
	SOCKET socketid = INVALID_SOCKET;
	socketid = socket(PF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN sockStruct;
	//使用TCP/IP协议
	sockStruct.sin_family = AF_INET;
	//绑定IP
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
			//连接指定IP计算机失败
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
			//绑定端口失败
			closesocket(socketid);
			socketid = INVALID_SOCKET;
		}
		else
		{
			if (listen(socketid, SOMAXCONN) == SOCKET_ERROR)
			{
				//监听函数调用失败
				closesocket(socketid);
				socketid = INVALID_SOCKET;
			}
		}
		m_Socket = socketid;
	}
	return socketid;
}

/*
功能：	创建一个新的套接字与参数addr指定的客户端套接字建立连接
参数：	s：处于监听状态的流套接字
		addr：新创建的套接字地址结构
		addrlen：新创建套接字地址结构的长度
返回值：调用失败：INVALID_SOCKET
		调用成功：可用的SOCKET
*/
SOCKET CTCP::myaccept(SOCKET s, struct sockaddr* addr, int* addrlen)
{
	SOCKET acceptsocket = INVALID_SOCKET;
	acceptsocket = accept(s, addr, addrlen);
	return acceptsocket;
}

/*
功能：	根据类型初始化SOCKET资源
参数：	sock：接收端套接字描述符
		buf：用来存放接收到的数据的缓冲区
		len：接收到的数据的大小
		flag：默认置零
		overtime：超时时间，秒为单位
		EndMark：结束标记
		Soonflag：是否立即返回
返回值：接收到的数据的字节数
*/
int CTCP::myrecv(SOCKET sock, char *buf, int len, int flag, UINT overtime,const char *EndMark, BOOL soonflag)
{
	int ret;
	int nLeft = len;
	int idx = 0;
	int nCount = 0;
	fd_set readfds;		//文件描述符的集合
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 500;
	DWORD s_time = GetTickCount();	//操作系统启动到现在的毫秒数
	while (nLeft > 0)
	{
		MSG msg;
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (msg.message == WM_QUIT)
			return 0;
		FD_ZERO(&readfds);//将set清零
		FD_SET(sock, &readfds);//将fd加入set

		if (select(0, &readfds, NULL, NULL, &timeout) == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		DWORD e_time = GetTickCount();
		if (!FD_ISSET(sock, &readfds))
		{
			if (e_time - s_time > overtime * 1000)	//超时
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
功能：	用指定的Socket发送数据
参数：	socket:发送端套接字描述符
		buf：用来存放要发送数据的缓冲区
		flag：一般置零
		overtime：设置超时时间
返回值：实际发送数据的字节数
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
			OutputDebugString(L"mysend  socket错误");
			return SOCKET_ERROR;
		}
		DWORD e_time = GetTickCount();
		if (!FD_ISSET(sock, &readfds))
		{
			if (e_time - s_time > overtime * 1000)	//超时
			{
				OutputDebugString(L"mysend 发送数据超时");
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