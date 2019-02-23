#pragma once

//检测注入攻击
bool check_sql(char *buf,char *fpath);

//检测xss攻击
bool check_xss(char *buf,char *fpath);


//限制上传文件
bool check_upload(string buf,char *fpath);


//检测暴力破解
bool check_bruteforce(char *buf,char *fpath);