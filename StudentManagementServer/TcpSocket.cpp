#include "TcpSocket.h"
#include <assert.h>

TcpSocket::TcpSocket()
{
}

int TcpSocket::Update(TcpPcb* pPcb, int nCurrent)
{
	static int nTimes = 0;

	//a form of throttling to limit the frequency of updates.
	if (nCurrent - nTimes < 30) {
		return 0;
	}

	//����ȷ�ϰ�
	if (!pPcb->AckList.empty()) {
		for (auto seq : pPcb->AckList) {
			SendAck(pPcb, seq);
		}
		pPcb->AckList.clear();
	}

	//�����Ͷ��е������ƶ������ͻ�����
	if (!pPcb->SendQueue.empty()) {
		for (auto e : pPcb->SendQueue) {
			pPcb->SendBuf.push_back(e);
		}
		pPcb->SendQueue.clear();
		LOGD("[DEBUG] %s move SendQueue:%d SendBuf:%d\n",
			__FUNCTION__,
			pPcb->SendQueue.size(),
			pPcb->SendBuf.size());
	}


	//�������ͻ�������������ݡ�
	for (TcpPacket* p : pPcb->SendBuf) {
		//��ʱ�ط� RTO �ط�ʱ�� 40 

		//��һ�η���ֱ�ӷ�
		if (p->nCnt == 0) {
			Output(pPcb, p);
			p->nCnt++;
		}
		else if (p->nCnt > 1000) {
			//�Ͽ�
		}
		//�����ط�
		else if ((nCurrent - p->nTime) > pPcb->nRto) {
			Output(pPcb, p);
			p->nCnt++;
		}

		p->nTime = nCurrent;

	}
	//pPcb->SendBuf.clear();

	return 0;
}

TcpSocket::~TcpSocket()
{

}

int TcpSocket::PeekSize(TcpPcb* pPcb)
{
	int nSize = 0;
	bool IsEnd = false;
	for (auto p : pPcb->RecvQueue) {
		nSize += p->nLen;
		if (p->nSegment == 0) {//һ��������
			IsEnd = true;
			break;
		}
	}

	if (!IsEnd)
		return -1;

	return nSize;
}

int TcpSocket::InitSocket(TcpPcb* pPcb, int type, int protocol) {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		LOGE("WSAStartup");
		return -1;
	}
	LOGD("[DEBUG] WSAStartup version 2.2\n");

	pPcb->Socket = socket(AF_INET, type, protocol); //Ĭ��Э��
	if (pPcb->Socket < 0) {
		LOGE("socket");
		return -1;
	}
	LOGD("[DEBUG] socket:%d type:%d protocol:%d\n",
		pPcb->Socket, type, protocol);

	//��socket����Ϊ������ģʽ���첽)
	u_long nBlock = 1;
	if (ioctlsocket(pPcb->Socket, FIONBIO, &nBlock) < 0) {
		LOGE("ioctlsocket");
		return -1;
	}

	return 0;
}

unsigned int TcpSocket::SetConv()
{
	GUID guid;
	CoCreateGuid(&guid); // ����һ�������GUID
	unsigned int crc32 = RtlComputerCrc32(&guid, sizeof(guid));
	unsigned int conv = static_cast<unsigned int>(crc32);
	return conv;
}

unsigned int TcpSocket::RtlComputerCrc32(void* pData, int iLen)
{

	typedef unsigned int(__stdcall* pfnAPI)(int dwInitial, void* pData, int iLen);

	HMODULE tMod = LoadLibrary(L"ntdll.dll");
	if (tMod == NULL) {
		LOGE("LoadLibrary");
		return -1;  // ���߷����ʵ��Ĵ������
	}

	pfnAPI tProc = (pfnAPI)GetProcAddress(tMod, "RtlComputeCrc32");
	if (tProc == NULL) {
		LOGE("GetProcAddress");
		FreeLibrary(tMod); // �ͷ��Ѽ��صĿ�
		return -1;  // ���߷����ʵ��Ĵ������
	}
		unsigned int result = tProc(0, pData, iLen);

		// ��ɺ��ͷ��Ѽ��صĿ�
		FreeLibrary(tMod);

		return result;
	}



int TcpSocket::BindLocal(TcpPcb* pPcb, const char* pAddress, short nPort)
{
	pPcb->LocalAddr.sin_family = AF_INET;
	pPcb->LocalAddr.sin_addr.s_addr = inet_addr(pAddress);
	pPcb->LocalAddr.sin_port = htons(nPort);
	if (bind(pPcb->Socket,
		(sockaddr*)&pPcb->LocalAddr,
		sizeof(pPcb->LocalAddr)) < 0) {
		LOGE("bind");
		return -1;
	}
	return 0;
}

int TcpSocket::BindRemote(TcpPcb* pPcb, const char* pAddress, short nPort)
{
	pPcb->RemoteAddr.sin_family = AF_INET;
	pPcb->RemoteAddr.sin_addr.s_addr = inet_addr(pAddress);
	pPcb->RemoteAddr.sin_port = htons(nPort);
	return 0;
}

int TcpSocket::Listen(TcpPcb* pPcb, int nMaxConn)
{
	return 0;
}

int TcpSocket::Accept(TcpPcb* pPcb)
{
	return 0;
}

int TcpSocket::SendAck(TcpPcb* pPcb, int nSeq)
{
	TcpPacket* p = (TcpPacket*)new char[sizeof(TcpPacket) - 1];
	if (p == NULL)
		return -1;
	p->conv = pPcb->conv;
	p->nCmd = CMD_ACK;
	p->nSeq = pPcb->nSeq++;
	p->nAckSeq = nSeq; // Set to the provided sequence number (nSeq)
	p->nLen = 0;
	p->nSegment = 0;

	//�������뵽���Ͷ���
	pPcb->SendQueue.push_back(p);

	LOGD("[DEBUG] cmd:ACK seq:%d nAckSeq:%d\r\n",
		p->nSeq, p->nAckSeq);

	return 0;
}

int TcpSocket::Send(TcpPcb* pPcb, char* pBuf, int nLen)
{
	if (nLen < 0) {
		LOGE("nLen < 0!\n");
		return -1;
	}
	assert(nLen >= 0);

	LOGD("[DEBUG] Send pBuf:%p nLen:%d\r\n", pBuf, nLen);
	int nHeadSize = sizeof(TcpPacket) - 1;
	//���ݰ���Ƭ
	//�����Ƭ����  1400  1390
	int nCount = nLen % (TCP_MSS - nHeadSize) == 0 ?
		nLen / (TCP_MSS - nHeadSize) :
		nLen / (TCP_MSS - nHeadSize) + 1;
	int nSent = 0;
	for (int i = 0; i < nCount; i++) {
		int nMssLen = (nLen - nSent) > (TCP_MSS - nHeadSize)
			? (TCP_MSS - nHeadSize) : (nLen - nSent);
		TcpPacket* p = (TcpPacket*)new char[nMssLen + nHeadSize];
		if (p == NULL)
			return -1;
		/*
		1400

		1400
		10   1400 - 10 = 1390

		nSent = 1390';
		*/
		p->conv = pPcb->conv;
		p->nCmd = CMD_PSH;
		p->nSeq = pPcb->nSeq++;
		p->nLen = nMssLen;
		p->nTime = 0;
		p->nCnt = 0;
		p->nSegment = nCount - i - 1;  //3  0 1 2   2 1 0 //nSegment�������š���������ʱ��0��֪���ǰ��Ľ�β��
		memcpy(p->Data, pBuf + nSent, nMssLen);

		//�������뵽���Ͷ���
		pPcb->SendQueue.push_back(p);

		LOGD("[DEBUG] conv: %d cmd:%d seq:%d segment:%d nLen:%d nSent:%d SendQueue:%d SendBuf:%d\r\n",
			p->conv,
			p->nCmd,
			p->nSeq,
			p->nSegment,
			p->nLen,
			nSent,
			pPcb->SendQueue.size(),
			pPcb->SendBuf.size());

		nSent += nMssLen;
	}

	return nSent;
}

int TcpSocket::Recv(TcpPcb* pPcb, char* pBuf, int nLen)
{
	//�ж����ݹ�����
	if (nLen < 0)
		return -1;



	//0 1 2 3 4 => 4 3 2 1 0  nSegment������д����������0��֪��һ��������
	//��ȡ�������ݴ�С hello  100(���յ�λһ�����ݰ�)
	int nSize = PeekSize(pPcb); //��RecvQueue���ȡһ���������ݰ��Ĵ�С��
	int nCopy = 0;
	if (nSize > 0) {
		if (nSize > nLen) {
			return -2; //��������С����
		}

		//��������
		for (auto it = pPcb->RecvQueue.begin();
			it != pPcb->RecvQueue.end() && nCopy < nSize;  //����һ���������ݵ�������pBuf
			) {
			memcpy(pBuf + nCopy, (*it)->Data, (*it)->nLen);
			nCopy += (*it)->nLen;

			LOGD("[DEBUG] recv seq:%d segment:%d nLen:%d\r\n",
				(*it)->nSeq,
				(*it)->nSegment,
				(*it)->nLen);

			delete (*it); //�ͷ���Դ
			it = pPcb->RecvQueue.erase(it);
		}
	}

	//���������飨�ȷ�����)
	for (auto it = pPcb->RecvBuf.begin();
		it != pPcb->RecvBuf.end();
		) {
		//�ж��Ƿ���Ҫ�İ�
		if ((*it)->nSeq == pPcb->nRecvSeq) {
			//������ն���
			pPcb->RecvQueue.push_back(*it);
			pPcb->nRecvSeq++; //��һ�����
			it = pPcb->RecvBuf.erase(it);   //it�Զ������һ��it������++����д������
		}
		else {
			it++;
		}
	}

	return nCopy;
}

int TcpSocket::Connect(TcpPcb* pPcb, const char* pAddress, short nPort)
{
	return 0;
}

int TcpSocket::Close(TcpPcb* pPcb)
{
	closesocket(pPcb->Socket);
	WSACleanup();
	return 0;
}

TcpPcb* TcpSocket::AllocPcb()
{
	TcpPcb* pPcb = new TcpPcb();
	pPcb->conv = SetConv();
	return pPcb;
}

int TcpSocket::FreePcb(TcpPcb* pPcb)
{
	delete pPcb;
	return 0;
}

int TcpSocket::Input(TcpPcb* pPcb)
{
	//��ȡ����
	char RcvBuf[TCP_MSS];
	int nLen = sizeof(pPcb->RemoteAddr);
	int nBytes = recvfrom(pPcb->Socket, RcvBuf, sizeof(RcvBuf), 0,
		(sockaddr*)&pPcb->RemoteAddr,
		&nLen);
	if (nBytes < 0) {
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return 0;

		LOGE("recvfrom");
		return -1;
	}



	//��������
	switch (((TcpPacket*)RcvBuf)->nCmd) {
	case CMD_PSH: {

		//�����ݷ��뵽���ջ�����(����֤˳��)
		TcpPacket* p = (TcpPacket*)new char[TCP_MSS];
		memcpy(p, RcvBuf, sizeof(RcvBuf));
		pPcb->RecvBuf.push_back(p);
		LOGD("[DEBUG] conv: %d cmd:PSH seq:%d segment:%d nLen:%d RcvBuf:%d\r\n",
			p->conv,
			p->nSeq,
			p->nSegment,
			p->nLen,
			pPcb->RecvBuf.size());

		//����Ҫȷ�ϰ��������
		pPcb->AckList.push_back(p->nSeq);

		break;
	}
	case CMD_ACK: {
		TcpPacket* p = (TcpPacket*)RcvBuf;
		//�����Ѿ�ȷ�ϣ�ɾ�����ͻ�����������
		for (auto it = pPcb->SendBuf.begin();
			it != pPcb->SendBuf.end();) {
			if ((*it)->nSeq == p->nAckSeq) {
				it = pPcb->SendBuf.erase(it);
			}
			else {
				it++;
			}
		}
		break;
	}
	}

	return nBytes;
}

int TcpSocket::Output(TcpPcb* pPcb, TcpPacket* p)
{
	int nSent = sendto(pPcb->Socket,
		(char*)p,
		p->nLen + sizeof(TcpPacket) - 1,
		0,
		(sockaddr*)&pPcb->RemoteAddr, sizeof(pPcb->RemoteAddr));
	if (nSent < 0) {
		LOGE("sendto");
		return -1;
	}
	LOGD("[DEBUG] %s conv: %d cmd:%d seq:%d segment:%d nLen:%d nSent:%d\r\n",
		__FUNCTION__,
		p->conv,
		p->nCmd,
		p->nSeq,
		p->nSegment,
		p->nLen,
		nSent);
	return 0;
}

