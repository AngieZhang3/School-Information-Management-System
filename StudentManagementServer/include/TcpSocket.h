#pragma once

#include "log.h"
#include <list>

//1(0) 2(1) 3(0) 4(1) 5(0) 6(1)

#define CMD_PSH 1
#define CMD_ACK 2

#pragma pack(1)
struct TcpPacket {
  int nCmd;
  int nLen;
  int nSeq;
  int nAckSeq;
  int nSegment; //片的编号
  int nTime;
  int nCnt;  //发送次数
  char Data[1];
};

//segment
#define TCP_MTU 1500
#define TCP_MSS 1400 //分片最大值

//协议控制块
struct TcpPcb {
  TcpPcb() {
    nSeq = 0;
    nRecvSeq = 0;  //期望收的包
    Socket = -1;
    nRto = 1000;
  }

  int nSeq;
  int nRecvSeq; //期望接收下一个包
  int Socket;
  int nRto;  //重发时间

  //发送队列
  std::list<TcpPacket*>  SendQueue;

  //发送缓冲区
  std::list<TcpPacket*>  SendBuf;

  //接收队列
  std::list<TcpPacket*>  RecvQueue;

  //接收缓冲区
  std::list<TcpPacket*> RecvBuf;  

  //确认队列
  std::list<int> AckList;  

  sockaddr_in  LocalAddr;
  sockaddr_in RemoteAddr;
};


class TcpSocket
{
public:
  TcpSocket();
  ~TcpSocket();
  int SendAck(TcpPcb *pPcb, int nSeq);
  int PeekSize(TcpPcb* pPcb);
  int Update(TcpPcb* pPcb, int nCurrent);
  int BindLocal(TcpPcb* pPcb, const char* pAddress, short nPort);
  int BindRemote(TcpPcb* pPcb, const char* pAddress, short nPort);
  int Listen(TcpPcb* pPcb, int nMaxConn);
  int Accept(TcpPcb* pPcb);
  int Send(TcpPcb* pPcb, char* pBuf, int nLen);
  int Recv(TcpPcb* pPcb, char* pBuf, int nLen);
  int Connect(TcpPcb* pPcb, const char* pAddress, short nPort);
  int Close(TcpPcb *pPcb);
  TcpPcb*  AllocPcb();
  int  FreePcb(TcpPcb* pPcb);

  //输入输出数据
  int Input(TcpPcb* pPcb);
  int Output(TcpPcb* pPcb, TcpPacket* p);
public:
  int InitSocket(TcpPcb* pPcb, int type, int protocol);
};

