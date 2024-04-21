
#include"ThreadPool.h"

 //在构造函数中将  启动threadnum个线程
ThreadPool::ThreadPool(size_t threadnum,const std::string &threadtype):stop_(false),threadtype_(threadtype)
{
  //启动threadnum个线程，每个线程将阻塞在条件变量上
  for(size_t ii=0;ii<threadnum;ii++)
  {
    //用lambda函数创建线程
    threads_.emplace_back([this]
    {
      printf("create %s thread(%ld).\n",threadtype_.c_str(),syscall(SYS_gettid));     //显示线程ID
      
      while(stop_==false)
      {    
        std::function<void()>task;   //用于存放出队的元素
       
         //////////////锁作用域的开始////////////////
        {
          std::unique_lock<std::mutex>lock(this->mutex_);
    
          //等待生产者的条件变量
           this->condition_.wait(lock,[this]
           {
              return ((this->stop_==true)||(this->taskqueue_.empty()==false));
            });
    
           if((this->stop_==true)&&(this->taskqueue_.empty()==true)) return;
    
           task=std::move(this->taskqueue_.front());
           this->taskqueue_.pop();
         }
        ///////////////锁作用域的结束/////////////////
    
       task(); 
      }
  });
 }
}
  
  //把任务添加到队列中
void ThreadPool::addtask(std::function<void()>task)
{
  { //////////锁的作用域开始//////////////
    std::lock_guard<std::mutex>lock(mutex_);
    taskqueue_.push(task);
  } //////////锁的作用域结束//////////////
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
  
  //等待全部线程执行完任务后退出
  for(std::thread &th:threads_)
  {
    th.join();
  }
}

  //在析构函数中将停止线程
ThreadPool::~ThreadPool()
{
  stop();
}









