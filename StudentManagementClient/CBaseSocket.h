
#pragma once
#pragma warning(disable:4996)
#include <Winsock2.h>
#pragma comment(lib, "Ws2_32.lib")


class CBaseSocket
{
public:
  int SetError(const char* pName) {
    m_nErrCode = WSAGetLastError();

    LPVOID lpMsgBuf;
    FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        m_nErrCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL 
    );

    m_szError[0] = '\0';
    strncat_s(m_szError, "[", sizeof(m_szError));
    strncat_s(m_szError, pName, sizeof(m_szError));
    strncat_s(m_szError, "]", sizeof(m_szError));
    strncat_s(m_szError, (char*)lpMsgBuf, sizeof(m_szError));

    LocalFree( lpMsgBuf );
    return 0;
  }

  DWORD GetErrCode() {
    return m_nErrCode;
  }
  const char* GetErrMsg() {
    return m_szError;
  }

  CBaseSocket() {
    m_Socket = INVALID_SOCKET;
  }
  CBaseSocket(SOCKET s) {
    m_Socket = s;
  }
  
  virtual ~CBaseSocket() {
     Close();
  }

  int CreateSocket(int nType, int nProto) {
    m_Socket = socket(AF_INET, nType, nProto);
    if (m_Socket == INVALID_SOCKET) {
      SetError("socket");    
      return -1;
    }
    return 0;
  }

  int Bind(const char* pAddress, int nPort) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(nPort);
    addr.sin_addr.s_addr = inet_addr(pAddress);
    if (bind(m_Socket, (sockaddr *)&addr, sizeof(addr)) < 0) {
      SetError("bind");
      return -1;
    }
    return 0;
  }

  void Close() {
    if (m_Socket != INVALID_SOCKET) {
      closesocket(m_Socket);
      m_Socket = INVALID_SOCKET;
    }
  }

  SOCKET GetSocket() {
    return m_Socket;
  }
protected:
  SOCKET  m_Socket;
  static DWORD m_nErrCode;
  static char m_szError[0x1000];
};

