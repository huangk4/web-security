#include "stdafx.h"



//�洢�����ļ�·��
vector<string> filepath;
extern char g_virus_check_ip[MAX_PATH];//�����Ƽ�������IP
extern int g_virus_check_port;//�����Ƽ��������˿�


/*----------------------------
* ���� : �ݹ�����ļ��У��ҵ����а����������ļ�
*----------------------------
* ���� : find
* ���� : public
*
* ���� : lpPath [in]      ��������ļ���Ŀ¼
* ���� : fileList [in]    ���ļ����Ƶ���ʽ�洢��������ļ�
*/
void find(char* lpPath, std::vector<std::string> &fileList)
{
	char szFind[MAX_PATH];
	WIN32_FIND_DATAA FindFileData;

	strcpy(szFind, lpPath);
	strcat(szFind, "\\*.*");

	HANDLE hFind = ::FindFirstFileA(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)    return;


	string path = lpPath;//����ļ���·��
	while (true)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				char szFile[MAX_PATH];
				strcpy(szFile, lpPath);
				strcat(szFile, "\\");
				strcat(szFile, (char*)(FindFileData.cFileName));
				find(szFile, fileList);//�ݹ�			
			}
		}
		else
		{
			path = lpPath;
			path = path + "\\" + FindFileData.cFileName;
			fileList.push_back(FindFileData.cFileName);
			filepath.push_back(path);
		}
		if (!FindNextFileA(hFind, &FindFileData))
			break;
	}
	FindClose(hFind);
}



C_Virus::C_Virus(string hashpath)
{
	m_vhashpath = hashpath;
}

bool C_Virus::single_scan(char *file)
{
	char sha[256];
	FileSHA256(file, sha);
	fstream hashfile(m_vhashpath.c_str(), ios::in);
	if (!hashfile)
		return false;
	char viruhash[256];
	while (!hashfile.eof())
	{
		hashfile >> viruhash;
		if (strcmp(sha, viruhash) == 0)//ƥ��hashֵ
		{
			savelog(file);
			return true;
		}
	}
	return false;

}

bool *C_Virus::multi_scan(char *path,int &filenums)
{
	vector<string> fileList;//����һ����Ž���ļ����Ƶ�����  
	find(path, fileList);
	bool *check = new bool[filepath.size()];//�洢�����
	for (int i = 0; i < filepath.size(); i++)
	{

		char *temp_buf = new char[strlen(filepath[i].c_str()) + 1];
		strcpy(temp_buf, filepath[i].c_str());
		if (single_scan(temp_buf) == true)
			check[i] = true;
		else check[i] = false;
	}
	filenums = filepath.size();
	return check;
}

bool C_Virus::cloud_scan(char *file)
{
	CTCP client(2,2);
	SOCKET socket = client.InitSocket(SOCK_NOBIND,g_virus_check_ip ,g_virus_check_port );
	if (socket == INVALID_SOCKET)
	{
	MessageBox(NULL, L"socket��ʼ������", L"", MB_OK);
	exit(0);
	}
	char sha[256];
	FileSHA256(file, sha);
	fstream hashfile(m_vhashpath.c_str(), ios::in);
	if (!hashfile)
		return false;
	char viruhash[256];
	char result[4];
	client.mysend(socket, sha, 256);
	client.myrecv(socket, result, 4, 0);
	if (strcmp(result, "yes") == 0)
		return true;
	return false;
}
