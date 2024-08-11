#pragma once

#include "CBaseSocket.h"


class CTcpSocket :public CBaseSocket
{
public:
  CTcpSocket() {
  }
   CTcpSocket(SOCKET s)  :CBaseSocket(s){
  }
  ~CTcpSocket() {
  }

  int CreateSocket() {
    return CBaseSocket::CreateSocket(SOCK_STREAM, IPPROTO_TCP);
  }

  int Listen(int nBackLog = SOMAXCONN) {
    if (listen(m_Socket, nBackLog) < 0) {
      SetError("listen");
      return -1;
    }
    return 0;
  }

  int Connect(const char* pAddress, int nPort) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(nPort);
    addr.sin_addr.s_addr = inet_addr(pAddress);
    if (connect(m_Socket, (sockaddr *)&addr, sizeof(addr)) < 0) {
      SetError("connect");
      return -1;
    }
    return 0;
  }

  CTcpSocket* Accept(int nBackLog = SOMAXCONN) {
    sockaddr_in addr;
    int nLen = sizeof(addr);
    SOCKET s = accept(m_Socket, (sockaddr *)&addr, &nLen);
    if (s == INVALID_SOCKET) {
      SetError("accept");
      return NULL;
    }

    CTcpSocket *p = new CTcpSocket(s);
    if (p == NULL) {
      SetError("new CTcpSocket");
      return NULL;
    }
    return p;
  }

  int Sent(char *pBuf, int nLen) {
    int nSents = send(m_Socket, pBuf, nLen, 0);
    if (nSents < 0) {
       SetError("send");
      return -1;
    }
    return nSents;
  }

  int Receive(char *pBuf, int nLen) {
    int nBytes = recv(m_Socket, pBuf, nLen, 0);
    if (nBytes < 0) {
       SetError("recv");
      return -1;
    }
    return nBytes;
  }

//  bool SetNonBlocking() {
//#ifdef _WIN32
//      unsigned long mode = 1;
//      return ioctlsocket(m_Socket, FIONBIO, &mode) == 0;
//#else
//      int flags = fcntl(m_Socket, F_GETFL, 0);
//      return fcntl(m_Socket, F_SETFL, flags | O_NONBLOCK) == 0;
//#endif
//  }
};

