#pragma once


class C_Virus
{
private:
	string m_vhashpath;

public:
	C_Virus(string hashpath);
	bool single_scan(char *file);
	bool *multi_scan(char *path,int &filenums);
	bool cloud_scan(char *file);
};




/*----------------------------
* 功能 : 递归遍历文件夹，找到其中包含的所有文件
* 函数 : find
* 访问 : public
* 参数 : lpPath [in]      需遍历的文件夹目录
* 参数 : fileList [in]    以文件名称的形式存储遍历后的文件
*/
void find(char* lpPath, std::vector<string> &fileList);