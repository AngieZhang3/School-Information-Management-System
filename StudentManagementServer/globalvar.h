#pragma once
#include "TcpSocket.h" 
#include "include/mysql.h"
#pragma comment(lib, "libmysql")

//extern MYSQL g_mysql;
extern TcpSocket g_socket;
extern TcpPcb* g_pPcb;
class globalvar
{
};

