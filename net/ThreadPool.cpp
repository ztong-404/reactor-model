
#include"ThreadPool.h"

 //�ڹ��캯���н�  ����threadnum���߳�
ThreadPool::ThreadPool(size_t threadnum,const std::string &threadtype):stop_(false),threadtype_(threadtype)
{
  //����threadnum���̣߳�ÿ���߳̽�����������������
  for(size_t ii=0;ii<threadnum;ii++)
  {
    //��lambda���������߳�
    threads_.emplace_back([this]
    {
      printf("create %s thread(%ld).\n",threadtype_.c_str(),syscall(SYS_gettid));     //��ʾ�߳�ID
      
      while(stop_==false)
      {    
        std::function<void()>task;   //���ڴ�ų��ӵ�Ԫ��
       
         //////////////��������Ŀ�ʼ////////////////
        {
          std::unique_lock<std::mutex>lock(this->mutex_);
    
          //�ȴ������ߵ���������
           this->condition_.wait(lock,[this]
           {
              return ((this->stop_==true)||(this->taskqueue_.empty()==false));
            });
    
           if((this->stop_==true)&&(this->taskqueue_.empty()==true)) return;
    
           task=std::move(this->taskqueue_.front());
           this->taskqueue_.pop();
         }
        ///////////////��������Ľ���/////////////////
    
       task(); 
      }
  });
 }
}
  
  //��������ӵ�������
void ThreadPool::addtask(std::function<void()>task)
{
  { //////////����������ʼ//////////////
    std::lock_guard<std::mutex>lock(mutex_);
    taskqueue_.push(task);
  } //////////�������������//////////////
  condition_.notify_one();  
}
  

size_t ThreadPool::size()
{
  return threads_.size();
}

void ThreadPool::stop()
{
  if(stop_)return;
  stop_=true;
  
  condition_.notify_all(); 
  
  //�ȴ�ȫ���߳�ִ����������˳�
  for(std::thread &th:threads_)
  {
    th.join();
  }
}

  //�����������н�ֹͣ�߳�
ThreadPool::~ThreadPool()
{
  stop();
}









