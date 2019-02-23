#pragma once

class Cc_log:public CTCP
{
private:
	SOCKET socket;
	string type;
public:
	Cc_log(string type,int lowver, int higver, string strBindIp, u_short BindPort, int opt=0);
	~Cc_log();

public:
	bool Send_log(string buf,string time);
	bool download_log();
};