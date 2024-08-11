#pragma once
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable> //��������
#include <stdint.h>
#include <atomic>

#ifdef  _DEBUG
  #define LOGD(fmt,...)   printf(fmt, __VA_ARGS__)
  #define LOGE(fmt,...)  printf(fmt, __VA_ARGS__)
#else
  #define   LOGD(fmt,...) 
  #define   LOGE(fmt,...)
#endif //  DEBUG

class ITask {
public:
  //ִ��
  virtual void DoTask() = 0;
};

class CThreadPool
{
public:
  CThreadPool();
  CThreadPool(uint32_t nMaxThreadCount);
  ~CThreadPool();
  int AddTask(ITask* pTask);      //�������
protected:
  static void WorkThread(CThreadPool *pPool, int nIndex); //�����߳�
  int Wait();
private:
  bool                    m_IsRunning;
  uint32_t                m_nActiveThreads; //������߳���
  std::list<std::thread*> m_Threads; //�̳߳�
  std::mutex              m_TaskLock;  //ͬ������
  std::condition_variable m_CoundLock; //֪ͨ����
  std::list<ITask*>       m_TaskQueue; //�������
};


