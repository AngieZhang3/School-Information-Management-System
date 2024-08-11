#pragma once
//#include "TcpSocket.h" 
#include "CTcpSocket.h"
#include "include/mysql.h"
#include <mutex>
#pragma comment(lib, "libmysql")

extern MYSQL g_mysql;
//extern TcpSocket g_socket;
//extern TcpPcb* g_pPcb;
extern CTcpSocket g_tcpSocket;
extern std::mutex g_tcpSocketMutex;
class globalvar
{
};

