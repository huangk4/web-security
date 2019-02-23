#include "stdafx.h"

list<SOCKET> sock_list;//ȫ�������ſͻ������ӵ�SOCKET

//������Ϣȫ��config.txt�����·�����ж�ȡ
extern char g_sqlpath[MAX_PATH];//sqlע���������ļ�·��
extern char g_xsspath[MAX_PATH];//xss��������ļ�·��
extern char g_bpath[MAX_PATH];//upload�����������ļ�·��
extern char g_hpath[MAX_PATH];//upload�����������ļ�·��
extern bool g_checksql;
extern bool g_checkxss;
extern bool g_checkupload;
extern bool g_bai;
extern char g_logserver_ip[MAX_PATH];//����־������IP
extern int g_logport;//����־�������˿�

extern vector<string> g_sqlpayload;
extern vector<string> g_xsspayload;
extern vector<string> g_bainame;
extern vector<string> g_heiname;



/*����̲߳��������ݽṹ
���ݿͻ���socket
���ݴ�ż��������б�
*/
struct check_param
{
	list<SOCKET> *tsock_list;
	list<string> *tsql_reg;
};


WAF::WAF(int op,int wp, int lowver, int higver)	//���캯��,��ʼ��SOCKET
{

	open_port = op;
	web_port = wp;
	m_socket = INVALID_SOCKET;
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


	SOCKET socketid = INVALID_SOCKET;
	socketid = socket(PF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN sockStruct;
	//ʹ��TCP/IPЭ��
	sockStruct.sin_family = AF_INET;
	//��IP
	sockStruct.sin_addr.S_un.S_addr = INADDR_ANY;
	//�󶨶˿�
	sockStruct.sin_port = htons(op);


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
	m_socket = socketid;
}

WAF::~WAF()
{
}

void WAF::openwaf()//����WEB����ǽ
{


	ls = true;//��������

	SOCKADDR_IN addrClient;
	int lens = sizeof(SOCKADDR);

	SOCKET acceptsocket = INVALID_SOCKET;

	//��ʱ���¼���
	//UNDO...
	
	//������һֱ����ֱ�����ú����ı��־ls����������д���ļ�
	while (ls)
	{
		acceptsocket = accept(m_socket, (SOCKADDR *)&addrClient, &lens);//���տͻ�������
		sock_list.push_back(acceptsocket);
 		CreateThread(NULL, 0, check, &sock_list.back(), 0, NULL);//ÿ���յ������Ӿʹ����߳̽��м��
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
		MessageBox(NULL, L"sqlע�����ļ��޷���", L"����", MB_OK);
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

//����߳�
DWORD WINAPI check(LPVOID lparam)
{
	char content[buf_l];//�������ݣ��ɶ�ν��յ���buf�ϲ����
	const int buf_len = 0x100;
	char buf[buf_len + 1];
	memset(content, 0, buf_l);
	memset(buf, 0, buf_len+1);
	int bytesRecv;

	//���ý���ʱ��
	int nNetTimeout = 100;//0.1��
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
	//ֻ��Ҫ���HTTPͷ��POST���ּ���
	string get_head(content);
	UINT head_pos=get_head.find("\r\n\r\n");//HTTPͷ����λ��
	if (get_head[0] != 'G')//�����ΪGET��ʽ����
	{
		UINT post_pos = get_head.find("\r\n", head_pos + 4);//POST��������λ��
		if (post_pos == string::npos)
			post_pos = strlen(content);
		smallbuf = new char[post_pos];
	}
	else smallbuf = new char[head_pos];
	memcpy(smallbuf, content, strlen(smallbuf));


	string get_info = get_head.substr(0, get_head.find("\r\n", get_head.find("\r\n")+2));
	get_info.replace(0, get_info.find("\r"), " ");
	get_info.replace(0, get_info.find("\n"), " ");
	char *clientinfo=new char[get_info.length()];//HTTP�ͻ��˻�����Ϣ
	strcpy(clientinfo, get_info.c_str());

	
	if(g_checksql==true)//���SQLע��
		if (check_sql(smallbuf, g_sqlpath) == true)//ȷ��Ϊ����
		{
			savelog(clientinfo);
			sendlog(clientinfo, "1", g_logserver_ip, g_logport);
			//�Ƴ������ر�����
			sock_list.remove(*(SOCKET*)lparam);
			closesocket(*(SOCKET*)lparam);
			return 0;
		}
	if(g_checkxss==true)//���XSSע��
		if (check_xss(smallbuf, g_xsspath) == true)//ȷ��Ϊ����
		{
			savelog(clientinfo);
			sendlog(clientinfo, "2", g_logserver_ip, g_logport);
			//�Ƴ������ر�����
			sock_list.remove(*(SOCKET*)lparam);
			closesocket(*(SOCKET*)lparam);
			return 0;
		}

	if (g_checkupload == true)//���Ƿ��ļ��ϴ�
	{
		bool attack = false;
		UINT upload_pos = get_head.find("Content-Disposition:", head_pos + 4);//�ϴ��ļ�����λ��
		if (upload_pos != string::npos)//��������ϴ��ļ�
		{
			UINT upload_endpos = get_head.find("\r\n", upload_pos);//���ϵõ��ϴ��ļ�������Ϣ��һ����βλ��
			string upload_info = get_head.substr(upload_pos, upload_endpos - upload_pos);//��ȡ����
			if (g_bai == true)
				attack = check_upload(upload_info, g_bpath);
			else
				attack = check_upload(upload_info, g_hpath);
		}
		if (attack)
		{
			savelog(clientinfo);
			sendlog(clientinfo, "3", g_logserver_ip, g_logport);
			//�Ƴ������ر�����
			sock_list.remove(*(SOCKET*)lparam);
			closesocket(*(SOCKET*)lparam);
			return 0;
		}
	}
	

	
	//���û��⵽��������ת����WEB���񣬲���WEB�������ݷ��͸��ͻ���
	char * s_content=ex_web(content);
	send(*(SOCKET*)lparam,s_content ,strlen(s_content), 0);
	//�Ƴ������ر�����
	sock_list.remove(*(SOCKET*)lparam);
	closesocket(*(SOCKET*)lparam);
	return 0;

}

//����ȫ����ת����web���񲢽��շ�������
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

		SOCKADDR_IN socketServer;//������
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
	//���ý���ʱ��
	int nNetTimeout = 100;//0.1��
	//����web���񷵻ص����ݴ�ŵ�content��
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