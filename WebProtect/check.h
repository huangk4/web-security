#pragma once

//���ע�빥��
bool check_sql(char *buf,char *fpath);

//���xss����
bool check_xss(char *buf,char *fpath);


//�����ϴ��ļ�
bool check_upload(string buf,char *fpath);


//��Ⱪ���ƽ�
bool check_bruteforce(char *buf,char *fpath);