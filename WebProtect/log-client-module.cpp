// log-client-module.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

extern char g_vhashpath[MAX_PATH];//病毒木马检测配置文件路径
extern char g_logserver_ip[MAX_PATH];//云日志服务器IP
extern int g_logport;//云日志服务器端口
extern char g_virus_check_ip[MAX_PATH];//病毒云检测服务器IP
extern int g_virus_check_port;//病毒云检测服务器端口


//发送日志到云端
void sendlog(string log_content,string type,string ip,int port)
{
	Cc_log Zhu(type, 2, 2, ip,port);


	time_t now = time(0);
	tm *ltm = localtime(&now);
	string nowsec = "";
	char strsec[3];
	_itoa_s(ltm->tm_hour + 1900, strsec, sizeof(strsec), 10);
	nowsec = nowsec + strsec + ":";
	_itoa_s(ltm->tm_min + 1, strsec, sizeof(strsec), 10);
	nowsec = nowsec + strsec + ":";
	_itoa_s(ltm->tm_sec, strsec, sizeof(strsec), 10);
	nowsec = nowsec + strsec + ":";
	Zhu.Send_log(log_content, nowsec);



	//unsigned int length = 10;
	//unsigned int nums = log_content.length() / length;
	//string *part = new string[nums];
	//int t_pos = 0;
	//for (UINT i = 0; i < nums; i++)
	//{
	//	part[i] = log_content.substr(t_pos, length);
	//	t_pos += length;
	//	

	//	cout << part[i] << endl;
	//}

}

//下载云端日志
void downloadlog(string ip)
{

}

//本次存储日志
void savelog(char *buf)
{
	_mkdir("log\\");
	string t_time = "log\\";
	// 基于当前系统的当前日期/时间
	time_t now = time(0);
	tm *ltm = localtime(&now);
	//int转为字符类型
	char tstr[5];
	_itoa_s(ltm->tm_year + 1900, tstr, sizeof(tstr), 10);
	t_time = t_time + tstr + "-";
	_itoa_s(ltm->tm_mon + 1, tstr, sizeof(tstr), 10);
	t_time = t_time + tstr;
	_itoa_s(ltm->tm_mday, tstr, sizeof(tstr), 10);

	_mkdir(t_time.c_str());
	string str1 = t_time + "\\" + tstr + ".log";
	fstream pfile;

	string nowsec = "";
	char strsec[3];
	_itoa_s(ltm->tm_hour, strsec, sizeof(strsec), 10);
	nowsec = nowsec + strsec + ":";
	_itoa_s(ltm->tm_min, strsec, sizeof(strsec), 10);
	nowsec = nowsec + strsec + ":";
	_itoa_s(ltm->tm_sec, strsec, sizeof(strsec), 10);
	nowsec = nowsec + strsec+ " ";

	pfile.open(str1, ios::out | ios::app);
	pfile << nowsec << buf;
	pfile.close();
}