#pragma once


//要计算的字符，字符长度（去掉\0），hash结果
char* StrSHA256(const char* str, long long length, char* sha256);

//文件路径，hash结果
char* FileSHA256(const char* file, char* sha256);