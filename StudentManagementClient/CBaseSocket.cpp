#include "pch.h"
#include "CBaseSocket.h"

DWORD CBaseSocket::m_nErrCode = 0;
char CBaseSocket::m_szError[0x1000];