#include "stdafx.h"



//存储所有文件路径
vector<string> filepath;
extern char g_virus_check_ip[MAX_PATH];//病毒云检测服务器IP
extern int g_virus_check_port;//病毒云检测服务器端口


/*----------------------------
* 功能 : 递归遍历文件夹，找到其中包含的所有文件
*----------------------------
* 函数 : find
* 访问 : public
*
* 参数 : lpPath [in]      需遍历的文件夹目录
* 参数 : fileList [in]    以文件名称的形式存储遍历后的文件
*/
void find(char* lpPath, std::vector<std::string> &fileList)
{
	char szFind[MAX_PATH];
	WIN32_FIND_DATAA FindFileData;

	strcpy(szFind, lpPath);
	strcat(szFind, "\\*.*");

	HANDLE hFind = ::FindFirstFileA(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)    return;


	string path = lpPath;//存放文件夹路径
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
				find(szFile, fileList);//递归			
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
		if (strcmp(sha, viruhash) == 0)//匹配hash值
		{
			savelog(file);
			return true;
		}
	}
	return false;

}

bool *C_Virus::multi_scan(char *path,int &filenums)
{
	vector<string> fileList;//定义一个存放结果文件名称的链表  
	find(path, fileList);
	bool *check = new bool[filepath.size()];//存储检测结果
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
	MessageBox(NULL, L"socket初始化错误", L"", MB_OK);
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
