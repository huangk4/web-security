#include"stdafx.h"
extern bool g_bai;





//���ע�빥��
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
		if (regex_search(buf, sqlreg) == true)//��⵽sqlע��
			return true;
	}
	file.close();
	return false;
}
//���xss����
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
		if (regex_search(buf, xssreg) == true)//��⵽xssע��
			return true;
	}
	file.close();
	return false;
}

//�����ϴ��ļ�������������baiΪ������а�������⣬����Ϊ���������
bool check_upload(string buf,char *fpath)
{
	fstream file(fpath, ios::in);
	if (!file)
		return false;
	char regbuf[50];
	//������
	if (g_bai == false) 
	{
		while (!file.eof())
		{
			file.getline(regbuf, 50);
			regex extreg(regbuf);
			if (regex_search(buf, extreg) == true)//��⵽�Ƿ��ļ���׺
				return true;
		}
		file.close();
		return false;
	}
	//������
	else
	{
		while (!file.eof())
		{
			file.getline(regbuf, 50);
			regex extreg(regbuf);
			if (regex_search(buf, extreg) == true)//��⵽�Ϸ��ļ���׺
				return false;
		}
		file.close();
		return true;
	}
}

////��Ⱪ���ƽ�
//bool check_bruteforce(char *buf,char *fpath)
//{
//
//}