#include "CThreadPool.h"

//

CThreadPool::CThreadPool()
/*Get the number of CPU cores*/
:CThreadPool {std::thread::hardware_concurrency()}
{
}

CThreadPool::CThreadPool(uint32_t nMaxThreadCount)
{
  if (nMaxThreadCount == 0) {
    LOGE("[CThreadPool] Invalid Thread Count nMaxThreadCount:%d\n", nMaxThreadCount);
    return;
  }

  //create threads
  m_IsRunning = true;
  for (uint32_t i = 0; i < nMaxThreadCount; i++)
  {
    std::thread *p = new std::thread(WorkThread, this, i);
    m_Threads.push_back(p);
  }
  LOGD("[CThreadPool] Create OK nMaxThreadCount:%d\n", nMaxThreadCount);
  m_nActiveThreads = 0;
 
}

CThreadPool::~CThreadPool() {
  Wait();
}


//add task
int CThreadPool::AddTask(ITask *pTask)
{
  {
    //automatically release the lock when deconstructor is called
    std::unique_lock<std::mutex> Lock(m_TaskLock);
    if (pTask == nullptr) {
      LOGE("[AddTask] Invalid Task pTask:%p\n", pTask);
      return -1;
    }
    m_TaskQueue.push_back(pTask);
    LOGD("[AddTask] TaskQueue Size:%d\n", m_TaskQueue.size());
  }

  //deliver task signal 
  m_CoundLock.notify_one();

  return 0;
}

int CThreadPool::Wait()
{
  m_IsRunning = false;
  m_CoundLock.notify_all();

  //wait for all thread to end
  for (auto pThread : m_Threads) {
    pThread->join();
  }

  //release all tasks
  for (auto pTask : m_TaskQueue) {
    pTask->DoTask();
    delete pTask;
  }

  return 0;
}

void CThreadPool::WorkThread(CThreadPool *pPool, int nIndex)
{
    ITask *pTask = nullptr;

    LOGD("[WorkThread] Worker:%02d Start\n", nIndex);
    while (true) {
      LOGD("[WorkThread] Worker:%02d Wait Task\n", nIndex);
      {
        //get tasks
        std::unique_lock<std::mutex> Lock(pPool->m_TaskLock);

        //Wait for the task signal and the queue is not empty, otherwise the thread is suspended
        pPool->m_CoundLock.wait(Lock, [&]{return !pPool->m_TaskQueue.empty() || !pPool->m_IsRunning; });

        if (!pPool->m_IsRunning)
          break;

        if (!pPool->m_TaskQueue.empty()) {
          pTask = std::move(pPool->m_TaskQueue.front());
          pPool->m_TaskQueue.pop_front();
        }
        LOGD("[WorkThread] Worker:%02d Get Task\n", nIndex);
      }

 
      pPool->m_nActiveThreads++;

      LOGD("[WorkThread] Worker%02d DoTask m_nActiveThreads:%d\n",
            nIndex, pPool->m_nActiveThreads);

      //carry out the task
      pTask->DoTask();
      delete pTask;


      pPool->m_nActiveThreads--;

      LOGD("[WorkThread] Worker%02d EndTask m_nActiveThreads:%d\n",
            nIndex, pPool->m_nActiveThreads);
    }


    LOGD("[WorkThread] Worker:%02d Exit\n", nIndex);
}


