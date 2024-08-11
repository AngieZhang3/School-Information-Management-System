// StudentManagementServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
//#include "TcpSocket.h"
#include"CMySqlTask.h"



//#include "globalvar.h"




int InitSocket(int &inputSocket, int type, int protocol) {

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		LOGE("WSAStartup");
		return -1;
	}
	LOGI("WSAStartup");

	inputSocket = socket(AF_INET, type, protocol);
	if (inputSocket < 0) {
		LOGE("InitSocket");
		return -1;
	}
	LOGI("initSocket");


	return 0;
}

int UninitSocket(int socket)
{
	shutdown(socket, SD_RECEIVE);
	closesocket(socket);
	WSACleanup();
	LOGI("UnInitSocket");
	return 0;
}
int main()
{
	


	int listenSocket;
	try {
		if (InitSocket(listenSocket, SOCK_STREAM, IPPROTO_TCP) != 0) {
			throw std::runtime_error("Failed to initialize socket");
		}

		sockaddr_in SockAddr;
		SockAddr.sin_family = AF_INET;
		SockAddr.sin_port = htons(16666);
		SockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		int nAddrLen = sizeof(SockAddr);

		if (bind(listenSocket, (sockaddr*)&SockAddr, sizeof(SockAddr)) < 0) {
			throw std::runtime_error("binding failed");
		}

		if (listen(listenSocket, SOMAXCONN) < 0) {
			throw std::runtime_error("listening failed");
		}

		CThreadPool pool;

		sockaddr_in ClientAddr;
		int nAddLen = sizeof(ClientAddr);

		while (true) {
			int clientSocket = accept(listenSocket, (sockaddr*)&ClientAddr, &nAddLen);
			if (clientSocket == SOCKET_ERROR) {
		
					LOGE("accept");
					continue;
			}
			OutputDebugString("[StudentManagement][+] Accept ok");
			try {
				pool.AddTask(new CMySqlTask(clientSocket));
				OutputDebugString("[StudentManagement][+] AddTask ok");

			}
			catch (const std::exception& e) {
				LOGE("Task addition exception: ", e.what());
				//Handle exceptions according to application logic, such as closing clientSocket
				closesocket(clientSocket);
			}
		}
	}
	catch (const std::exception& e) {
		LOGE("main program exception: ", e.what());
		UninitSocket(listenSocket);
		return -1;
	}

#if 0
	TcpSocket s;
	TcpPcb* pPcb = s.AllocPcb();
	//printf("conv is %u\r\n", pPcb->conv);
	if (s.InitSocket(pPcb, AF_INET, IPPROTO_UDP) != 0)
	{
		LOGE("InitSocket");
		return -1;
	}
	if (s.BindLocal(pPcb, "127.0.0.1", 16666) < 0) {
		LOGE("BindLocal");
		return -1;
	}


	//char RcvBuf[5000];
	int nBytes = 0;
	while (true)
	{
		int nCurrent = GetTickCount();
		s.Update(pPcb, nCurrent);
		s.Input(pPcb);

		stPacketHdr hdr = { 0 };
		nBytes = s.Recv(pPcb, (char*)&hdr, sizeof(hdr));
		if (nBytes < 0)
		{
			LOGE("Recv");
			return -1;
		}
		if (nBytes > 0)
		{
			int nLen = hdr.nLen;
			int nCmd = hdr.nCmd;
			printf("Recv nBytes: %d  nLen = %d, nCmd = %d\r\n", nBytes, nLen, nCmd);
		}

		switch (hdr.nCmd) {
		case MYSQL_LOGIN:
			if (hdr.nLen > 0)
			{
				char* pRcvBuf = new char[hdr.nLen];
				nBytes = s.Recv(pPcb, pRcvBuf, hdr.nLen);
				if (nBytes < 0)
				{
					LOGE("Recv");
					return -1;
				}
				if (nBytes > 0)
				{
					printf("Recv nBytes: %d  data: %s\r\n", nBytes, pRcvBuf);
				}

			}
			break;

		}
		//	if (nBytes > 0)
		//		printf("Recv nBytes: %d data:%s\n", nBytes, RcvBuf);
	}
	s.Close(pPcb);
	s.FreePcb(pPcb);
#endif // udp

	return 0;
}



