#include"stdafx.h"
extern bool g_bai;





//检测注入攻击
bool check_sql(char *buf, char *fpath)
{
	fstream file(fpath, ios::in);
	if (!file)
		return false;
	char regbuf[100];
	while (!file.eof())
	{
		file.getline(regbuf,100);
		regex sqlreg(regbuf);
		if (regex_search(buf, sqlreg) == true)//检测到sql注入
			return true;
	}
	file.close();
	return false;
}
//检测xss攻击
bool check_xss(char *buf,char *fpath)
{
	fstream file(fpath, ios::in);
	if (!file)
		return false;
	char regbuf[100];
	while (!file.eof())
	{
		file.getline(regbuf, 100);
		regex xssreg(regbuf);
		if (regex_search(buf, xssreg) == true)//检测到xss注入
			return true;
	}
	file.close();
	return false;
}

//限制上传文件，第三个参数bai为真则进行白名单检测，否则为黑名单检测
bool check_upload(string buf,char *fpath)
{
	fstream file(fpath, ios::in);
	if (!file)
		return false;
	char regbuf[50];
	//黑名单
	if (g_bai == false) 
	{
		while (!file.eof())
		{
			file.getline(regbuf, 50);
			regex extreg(regbuf);
			if (regex_search(buf, extreg) == true)//检测到非法文件后缀
				return true;
		}
		file.close();
		return false;
	}
	//白名单
	else
	{
		while (!file.eof())
		{
			file.getline(regbuf, 50);
			regex extreg(regbuf);
			if (regex_search(buf, extreg) == true)//检测到合法文件后缀
				return false;
		}
		file.close();
		return true;
	}
}

////检测暴力破解
//bool check_bruteforce(char *buf,char *fpath)
//{
//
//}