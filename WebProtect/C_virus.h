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
* ���� : �ݹ�����ļ��У��ҵ����а����������ļ�
* ���� : find
* ���� : public
* ���� : lpPath [in]      ��������ļ���Ŀ¼
* ���� : fileList [in]    ���ļ����Ƶ���ʽ�洢��������ļ�
*/
void find(char* lpPath, std::vector<string> &fileList);