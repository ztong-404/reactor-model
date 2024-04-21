#pragma once

#include<functional>
#include<memory>
#include<unistd.h>
#include<queue>
#include<map>
#include<mutex>
#include<atomic>
#include<functional>
#include<sys/eventfd.h>
#include<sys/syscall.h>
#include<sys/timerfd.h>

#include"Connection.h"
#include"Epoll.h"

//checked
/*
事件循环类


*/
class Channel;
class Epoll;
class Connection;
using spConnection=std::shared_ptr<Connection>;

//事件循环类
class EventLoop
{
private:
  int timetvl_;                                     //闹钟时间间隔，单位：秒
  int timeout_;                                     //Connection对象超时的时间，单位：秒
  int wakeupfd_;                                    //用于唤醒事件循环线程的eventfd
  int timerfd_;                                     //定时器的fd
  bool mainloop_;                                   //true是主事件循环，false是从事件循环
  std::atomic_bool stop_;                           //初始值为false, 如果设置为true，表示停止事件循环
  pid_t threadid_;                                  //事件循环所在线程的id
  std::unique_ptr<Epoll>ep_;                       //每个事件循环只有一个Epoll
  std::unique_ptr<Channel> wakechannel_;           //eventfd的Channel                         
  std::unique_ptr<Channel>timerchannel_;            //定时器的channel
  std::mutex mutex_;                               //任务队列同步的互斥锁
  std::mutex mmutex_;                                //保护conns_的互斥锁
  
  std::queue<std::function<void()>>taskqueue_;     //事件循环线程被eventfd唤醒后执行的任务队列
  std::map<int,spConnection>conns_;                 //存放运行在该事件循环上全部的Connection对象 
  std::function<void(int)>timercallback_;              //删除TcpServer中超时的Connection对象，将被设置为TcpServer：：removeconn()
  std::function<void(EventLoop*)>epolltimeoutcallback_;   //epoll_wait()超时的回调函数
 
public:
  EventLoop(bool mainloop, int timetvl=30, int timeout=80);  //构造函数中创建epoll对象
  ~EventLoop(); //析构函数中销毁epoll对象

  void run(); //运行事件循环
  void stop(); //停止事件循环
  
  void updatechannel(Channel *ch);  //把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件
  void removechannel(Channel *ch);  //从红黑树上删除channel
  void setepolltimeoutcallback(std::function<void(EventLoop*)>fn); //设置eoll_wait（）超时的回调函数
  
  bool isinloopthread();                       //判断当前线程是否为事件循环线程。

  void queueinloop(std::function<void()>fn);   //把任务添加到队列中
  void wakeup();                               //用eventfd唤醒事件循环线程   
  void handlewakeup();                         //事件循环线程被evenfd唤醒后执行的函数 
  void handletimer();                          //闹钟响时执行的函数
  
  void newconnection(spConnection conn);       //把Connection对象保存在conns_中
  void settimercallback(std::function<void(int)>fn);   //将被设置为TcpServer::removeconn()
};