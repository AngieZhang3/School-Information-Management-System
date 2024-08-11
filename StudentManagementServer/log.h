#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <Winsock2.h>
//#include <ws2tcpip.h>
//#pragma comment(lib, "Ws2_32.lib")


#define DEBUG
#ifdef DEBUG
#define LOGD(fmt, ...)  printf(fmt, __VA_ARGS__)
#else
#define LOGD(fmt, ...) 
#endif

#define LOGI(func)  { \
  printf("[+] %-20s %s ok\n", __FUNCTION__,func);  \
}

#define LOGW(func)  { \
  LPVOID lpMsgBuf = GetErrorMsg(); \
  printf("[-] %-20s %s Error:%-6d %s file:%s line:%d\n", \
       __FUNCTION__, \
       func, \
       WSAGetLastError(), \
       (char*)lpMsgBuf, \
       __FILE__,  \
       __LINE__); \
  LocalFree(lpMsgBuf);\
}



#define LOGE(func)  { \
  LPVOID lpMsgBuf = GetErrorMsg(); \
  printf("[-] %-20s %s Error:%-6d %s file:%s line:%d\n", \
       __FUNCTION__, \
       func, \
       WSAGetLastError(), \
       (char*)lpMsgBuf, \
       __FILE__,  \
       __LINE__); \
  LocalFree(lpMsgBuf);\
}

//¼ÇÂ¼MySQL error
#define LOGE_A(func, errorMsg)  { \
  LPVOID lpMsgBuf = GetErrorMsg(); \
  printf("[-] %-20s %s Error:%-6d %s ErrorMsg:%s file:%s line:%d\n", \
       __FUNCTION__, \
       func, \
       WSAGetLastError(), \
       (char*)lpMsgBuf, \
       errorMsg, \
       __FILE__,  \
       __LINE__); \
  LocalFree(lpMsgBuf);\
}
LPVOID GetErrorMsg();

