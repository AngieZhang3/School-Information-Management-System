#pragma once
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable> //条件变量
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
  //执行
  virtual void DoTask() = 0;
};

class CThreadPool
{
public:
  CThreadPool();
  CThreadPool(uint32_t nMaxThreadCount);
  ~CThreadPool();
  int AddTask(ITask* pTask);      //添加任务
protected:
  static void WorkThread(CThreadPool *pPool, int nIndex); //工作线程
  int Wait();
private:
  bool                    m_IsRunning;
  uint32_t                m_nActiveThreads; //激活的线程数
  std::list<std::thread*> m_Threads; //线程池
  std::mutex              m_TaskLock;  //同步任务
  std::condition_variable m_CoundLock; //通知任务
  std::list<ITask*>       m_TaskQueue; //任务队列
};


