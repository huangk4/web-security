#include "stdafx.h"

Cc_log::Cc_log(string type, int lowver, int higver, string strBindIp, u_short BindPort, int opt ) :CTCP(lowver, higver)
{
	this->type = type;
	socket = InitSocket(SOCK_NOBIND, strBindIp, BindPort);
}

Cc_log::~Cc_log()
{
	
}

bool Cc_log::Send_log(string buf, string time)
{


	//�����׶˼�����־����ʱ��
	buf = type + time + buf;
	mysend(socket, buf.c_str(), buf.length());

	return true;
}