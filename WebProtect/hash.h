#pragma once


//Ҫ������ַ����ַ����ȣ�ȥ��\0����hash���
char* StrSHA256(const char* str, long long length, char* sha256);

//�ļ�·����hash���
char* FileSHA256(const char* file, char* sha256);