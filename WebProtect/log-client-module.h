#pragma once

//发送日志到云端
void sendlog(string log_content, string type, string ip, int port);

//下载云端日志
void downloadlog(string ip);

//本地存储日志
void savelog(char *buf);