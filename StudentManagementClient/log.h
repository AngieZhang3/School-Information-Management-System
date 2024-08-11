#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <sstream>
#include <Winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")


#define DEBUG
#ifdef DEBUG
#define LOGD(fmt, ...)  printf(fmt, __VA_ARGS__)
#else
#define LOGD(fmt, ...) 
#endif

#define LOGI(func)  { \
    CString strInfo; \
    strInfo.Format("[StudentManagement][+] %-20s %s ok\n", __FUNCTION__,func);\
  OutputDebugString(strInfo);  \
}


//Used to capture TcpSocket info
#define LOGI_S(func)  { \
    std::ostringstream ss; \
    ss << "[StudentManagement][+] " << __FUNCTION__ << " " << func << " ok\n"; \
    OutputDebugStringA(ss.str().c_str()); \
}

#define LOGE(func)  { \
  LPVOID lpMsgBuf = GetErrorMsg(); \
CString strError; \
  strError.Format("[StudentManagement][-] %-20s %s Error:%-6d %s file:%s line:%d\n", \
       __FUNCTION__, \
       func, \
       WSAGetLastError(), \
       (char*)lpMsgBuf, \
       __FILE__,  \
       __LINE__); \
    OutputDebugString(strError); \
  LocalFree(lpMsgBuf);\
}

// Used to capture TcpSocket error
#define LOGE_S(func)  { \
    LPVOID lpMsgBuf = GetErrorMsg(); \
    std::ostringstream ss; \
    ss << "[StudentManagement][-] " << __FUNCTION__ << " " << func << " Error:" << WSAGetLastError() << " " << static_cast<char*>(lpMsgBuf) << " file:" << __FILE__ << " line:" << __LINE__ << "\n"; \
    OutputDebugStringA(ss.str().c_str()); \
    LocalFree(lpMsgBuf); \
}



//Used to capture additional error codes, such as mysql
#define LOGE_A(func, errorMsg)  { \
  LPVOID lpMsgBuf = GetErrorMsg(); \
  CString strError; \
  strError.Format("[StudentManage][-] %-20s %s Error:%-6d %s %s file:%s line:%d\n", \
       __FUNCTION__, \
       func, \
       WSAGetLastError(), \
       (char*)lpMsgBuf, \
       errorMsg, \
       __FILE__,  \
       __LINE__); \
    OutputDebugString(strError); \
  LocalFree(lpMsgBuf);\
}


#define LOGW(func)  { \
  LPVOID lpMsgBuf = GetErrorMsg(); \
CString strWarning;\
strWarning.Format("[StudentManagement][-] %-20s %s Error:%-6d %s file:%s line:%d\n", \
    __FUNCTION__, \
    func, \
    WSAGetLastError(), \
    (char*)lpMsgBuf, \
    __FILE__, \
    __LINE__))\
  OutputDebugString(strWarning);\
  LocalFree(lpMsgBuf);\
}


LPVOID GetErrorMsg();

