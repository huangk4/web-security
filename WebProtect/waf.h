#pragma once

#define buf_l 0x4000

#define webport 81


//WEB防火墙类
class WAF
{
private:
	int open_port;//外界访问端口,默认为80
	int web_port;//实际WEB端口
	SOCKET m_socket;
	bool ls;//防火墙开关标志
	char buf[buf_l];//接收缓冲区
	list<string> sqlreg;
	list<string>xssreg;
	list<string>hreg;
	list<string>breg;


public:
	WAF(int op,int wp, int lowver=2, int higver=2);
	virtual ~WAF();
	void openwaf();//打开防火墙
	void closewaf();//关闭防火墙
	void get_sqlreg(char *path);//从文件中读取sql正则
	void get_xssreg(char *path);//从文件中读取xss正则
	void get_hreg(char *path);//从文件中读取黑名单
	void get_breg(char *path);//从文件中读取白名单
};

DWORD WINAPI check(LPVOID lparam);//WEB检测线程
char * ex_web(char *sendbuf);//将安全请求转发给web服务并接收返回内容

