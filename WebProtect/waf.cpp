#include "stdafx.h"

list<SOCKET> sock_list;//全局链表存放客户端连接的SOCKET

//下列信息全在config.txt（相对路径）中读取
extern char g_sqlpath[MAX_PATH];//sql注入检测配置文件路径
extern char g_xsspath[MAX_PATH];//xss检测配置文件路径
extern char g_bpath[MAX_PATH];//upload白名单配置文件路径
extern char g_hpath[MAX_PATH];//upload黑名单配置文件路径
extern bool g_checksql;
extern bool g_checkxss;
extern bool g_checkupload;
extern bool g_bai;
extern char g_logserver_ip[MAX_PATH];//云日志服务器IP
extern int g_logport;//云日志服务器端口

extern vector<string> g_sqlpayload;
extern vector<string> g_xsspayload;
extern vector<string> g_bainame;
extern vector<string> g_heiname;



/*检测线程参数的数据结构
传递客户端socket
传递存放检测参数的列表
*/
struct check_param
{
	list<SOCKET> *tsock_list;
	list<string> *tsql_reg;
};


WAF::WAF(int op,int wp, int lowver, int higver)	//构造函数,初始化SOCKET
{

	open_port = op;
	web_port = wp;
	m_socket = INVALID_SOCKET;
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


	SOCKET socketid = INVALID_SOCKET;
	socketid = socket(PF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN sockStruct;
	//使用TCP/IP协议
	sockStruct.sin_family = AF_INET;
	//绑定IP
	sockStruct.sin_addr.S_un.S_addr = INADDR_ANY;
	//绑定端口
	sockStruct.sin_port = htons(op);


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
	m_socket = socketid;
}

WAF::~WAF()
{
}

void WAF::openwaf()//启动WEB防火墙
{


	ls = true;//开启监听

	SOCKADDR_IN addrClient;
	int lens = sizeof(SOCKADDR);

	SOCKET acceptsocket = INVALID_SOCKET;

	//定时更新检测库
	//UNDO...
	
	//服务器一直监听直到调用函数改变标志ls，监听内容写入文件
	while (ls)
	{
		acceptsocket = accept(m_socket, (SOCKADDR *)&addrClient, &lens);//接收客户端连接
		sock_list.push_back(acceptsocket);
 		CreateThread(NULL, 0, check, &sock_list.back(), 0, NULL);//每次收到新连接就创建线程进行检测
	}

}

void WAF::closewaf()
{
	ls = false;
}

void WAF::get_sqlreg(char * path)
{
	fstream file(path, ios::in);
	if (!file)
	{	
		MessageBox(NULL, L"sql注入检测文件无法打开", L"错误", MB_OK);
		exit(0);
		return;
	}
	char regbuf[100];
	string temp;
	while (!file.eof())
	{
		file.getline(regbuf, 100);
		temp = regbuf;
		sqlreg.push_back(temp);
	}
	file.close();
}

void WAF::get_xssreg(char * path)
{
}

void WAF::get_hreg(char * path)
{
}

void WAF::get_breg(char * path)
{
}

//检测线程
DWORD WINAPI check(LPVOID lparam)
{
	char content[buf_l];//完整内容，由多次接收到的buf合并组成
	const int buf_len = 0x100;
	char buf[buf_len + 1];
	memset(content, 0, buf_l);
	memset(buf, 0, buf_len+1);
	int bytesRecv;

	//设置接收时限
	int nNetTimeout = 100;//0.1秒
	setsockopt(*(SOCKET*)lparam, SOL_SOCKET, SO_RCVTIMEO,(char *)&nNetTimeout, sizeof(int));
	int i = 0;
	while (1)
	{
		i++;
		bytesRecv = recv(*(SOCKET*)lparam, buf, buf_len, 0);
		if (bytesRecv <= 0)
			break;
		//printf("%s", buf);
		buf[bytesRecv] = '\0';
		StringCchCatA(content, buf_l, buf);

	}

	char *smallbuf;
	//只需要检测HTTP头和POST部分即可
	string get_head(content);
	UINT head_pos=get_head.find("\r\n\r\n");//HTTP头结束位置
	if (get_head[0] != 'G')//如果不为GET方式访问
	{
		UINT post_pos = get_head.find("\r\n", head_pos + 4);//POST参数结束位置
		if (post_pos == string::npos)
			post_pos = strlen(content);
		smallbuf = new char[post_pos];
	}
	else smallbuf = new char[head_pos];
	memcpy(smallbuf, content, strlen(smallbuf));


	string get_info = get_head.substr(0, get_head.find("\r\n", get_head.find("\r\n")+2));
	get_info.replace(0, get_info.find("\r"), " ");
	get_info.replace(0, get_info.find("\n"), " ");
	char *clientinfo=new char[get_info.length()];//HTTP客户端基本信息
	strcpy(clientinfo, get_info.c_str());

	
	if(g_checksql==true)//检测SQL注入
		if (check_sql(smallbuf, g_sqlpath) == true)//确认为攻击
		{
			savelog(clientinfo);
			sendlog(clientinfo, "1", g_logserver_ip, g_logport);
			//移出链表，关闭连接
			sock_list.remove(*(SOCKET*)lparam);
			closesocket(*(SOCKET*)lparam);
			return 0;
		}
	if(g_checkxss==true)//检测XSS注入
		if (check_xss(smallbuf, g_xsspath) == true)//确认为攻击
		{
			savelog(clientinfo);
			sendlog(clientinfo, "2", g_logserver_ip, g_logport);
			//移出链表，关闭连接
			sock_list.remove(*(SOCKET*)lparam);
			closesocket(*(SOCKET*)lparam);
			return 0;
		}

	if (g_checkupload == true)//检测非法文件上传
	{
		bool attack = false;
		UINT upload_pos = get_head.find("Content-Disposition:", head_pos + 4);//上传文件类型位置
		if (upload_pos != string::npos)//如果发现上传文件
		{
			UINT upload_endpos = get_head.find("\r\n", upload_pos);//由上得到上传文件基本信息的一行首尾位置
			string upload_info = get_head.substr(upload_pos, upload_endpos - upload_pos);//提取该行
			if (g_bai == true)
				attack = check_upload(upload_info, g_bpath);
			else
				attack = check_upload(upload_info, g_hpath);
		}
		if (attack)
		{
			savelog(clientinfo);
			sendlog(clientinfo, "3", g_logserver_ip, g_logport);
			//移出链表，关闭连接
			sock_list.remove(*(SOCKET*)lparam);
			closesocket(*(SOCKET*)lparam);
			return 0;
		}
	}
	

	
	//如果没检测到攻击，则转发给WEB服务，并将WEB返回内容发送给客户端
	char * s_content=ex_web(content);
	send(*(SOCKET*)lparam,s_content ,strlen(s_content), 0);
	//移出链表，关闭连接
	sock_list.remove(*(SOCKET*)lparam);
	closesocket(*(SOCKET*)lparam);
	return 0;

}

//将安全请求转发给web服务并接收返回内容
char * ex_web(char *sendbuf)
{
	BOOL bRet = FALSE;
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);

	SOCKET socketClient = INVALID_SOCKET;
	socketClient = socket(PF_INET, SOCK_STREAM, 0);
	if (socketClient == INVALID_SOCKET)
	{
		WSACleanup();
		return NULL;
	}

	do
	{
		if (0 != WSAStartup(wVersion, &wsaData))
		{
			break;
		}
		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
		{
			WSACleanup();
			break;
		}

		SOCKADDR_IN socketServer;//服务器
		socketServer.sin_family = AF_INET;
		socketServer.sin_port = htons(webport);
		socketServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

		if (SOCKET_ERROR == connect(socketClient, (LPSOCKADDR)&socketServer, sizeof(SOCKADDR_IN)))
		{
			int nErrorCode = WSAGetLastError();
			closesocket(socketClient);
			break;
		}

		bRet = TRUE;
	} while (FALSE);


	send(socketClient, sendbuf, strlen(sendbuf), 0);
	/*fstream file1("http.txt", ios::out|ios::app);
	file1 << sendbuf;
	file1.close();*/

	char *content=new char[buf_l];
	const int buf_len = 0x100;
	char buf[buf_len+1];
	memset(content, 0, buf_l);
	memset(buf, 0,buf_len);
	int bytesRecv;
	//设置接收时限
	int nNetTimeout = 100;//0.1秒
	//接收web服务返回的内容存放到content里
	while (1)
	{
		setsockopt(socketClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
		bytesRecv = recv(socketClient, buf, buf_len, 0);
		if (bytesRecv <= 0)
			break;
		buf[bytesRecv] = '\0';
		StringCchCatA(content,buf_l, buf);
	}
	//content[]
	return content;
}