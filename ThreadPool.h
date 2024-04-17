#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <sys/syscall.h>
#include <mutex>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

class ThreadPool
{
private:
  std::vector<std::thread>threads_;  //�̳߳��е��߳�
  std::queue<std::function<void()>>taskqueue_; //�������
  std::mutex mutex_;                           //�������ͬ���Ļ�����
  std::condition_variable condition_;          //�������ͬ������������
  std::atomic_bool stop_;                      //�����������У���stop_��ֵ����Ϊtrue��ȫ�����߳̽��˳�
  std::string threadtype_;                     //�߳����ࣺ��IO������WORKS��                  
  
public:
  //�ڹ��캯���н�  ����threadnum���߳�
  ThreadPool(size_t threadnum,const std::string &threadtype);
  
  //��������ӵ�������
  void addtask(std::function<void()>task);
  
  //�����������н�ֹͣ�߳�
  ~ThreadPool();
  
  //��ȡ�̳߳ش�С
  size_t size();
  
  //ֹͣ�߳�
  void stop();
  
};