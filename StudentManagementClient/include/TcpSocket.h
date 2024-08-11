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
  int nSegment; //Ƭ�ı��
  int nTime;
  int nCnt;  //���ʹ���
  char Data[1];
};

//segment
#define TCP_MTU 1500
#define TCP_MSS 1400 //��Ƭ���ֵ

//Э����ƿ�
struct TcpPcb {
  TcpPcb() {
    nSeq = 0;
    nRecvSeq = 0;  //�����յİ�
    Socket = -1;
    nRto = 1000;
  }

  int nSeq;
  int nRecvSeq; //����������һ����
  int Socket;
  int nRto;  //�ط�ʱ��

  //���Ͷ���
  std::list<TcpPacket*>  SendQueue;

  //���ͻ�����
  std::list<TcpPacket*>  SendBuf;

  //���ն���
  std::list<TcpPacket*>  RecvQueue;

  //���ջ�����
  std::list<TcpPacket*> RecvBuf;  

  //ȷ�϶���
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

  //�����������
  int Input(TcpPcb* pPcb);
  int Output(TcpPcb* pPcb, TcpPacket* p);
public:
  int InitSocket(TcpPcb* pPcb, int type, int protocol);
};

