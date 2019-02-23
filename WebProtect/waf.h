#pragma once

#define buf_l 0x4000

#define webport 81


//WEB����ǽ��
class WAF
{
private:
	int open_port;//�����ʶ˿�,Ĭ��Ϊ80
	int web_port;//ʵ��WEB�˿�
	SOCKET m_socket;
	bool ls;//����ǽ���ر�־
	char buf[buf_l];//���ջ�����
	list<string> sqlreg;
	list<string>xssreg;
	list<string>hreg;
	list<string>breg;


public:
	WAF(int op,int wp, int lowver=2, int higver=2);
	virtual ~WAF();
	void openwaf();//�򿪷���ǽ
	void closewaf();//�رշ���ǽ
	void get_sqlreg(char *path);//���ļ��ж�ȡsql����
	void get_xssreg(char *path);//���ļ��ж�ȡxss����
	void get_hreg(char *path);//���ļ��ж�ȡ������
	void get_breg(char *path);//���ļ��ж�ȡ������
};

DWORD WINAPI check(LPVOID lparam);//WEB����߳�
char * ex_web(char *sendbuf);//����ȫ����ת����web���񲢽��շ�������

