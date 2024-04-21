 #pragma once

#include<sys/epoll.h>
#include<functional>
#include<memory>

#include"EventLoop.h"
#include"InetAddress.h"
#include"Socket.h"
#include"Epoll.h"

/************************
Channel类

************************/


class EventLoop;

class Channel
{
private:
  int fd_=-1;                  //channel 拥有的fd， channel和fd是一对一的关系
  EventLoop* loop_;       //Channel对应的事件循环，Channel与EventLoop是多对一的关系，一个channel只对应一个EventLoop
  bool inepoll_=false;         //channel是否已添加到epoll树上，如果未添加，调用epoll_ctl()的EPOLL_CTL_ADD,否则用EPOLL_CTL_MOD
  uint32_t events_=0;         //fd_需要监视的事件. listenfd和clientfd需要监视EPOLLIN,clientfd还肯需要监视EPOLLOUT
  uint32_t revents_=0;       //fd_已发生的事件
  std::function<void()>readcallback_;  //fd_读事件的回调函数， 如果是acceptchannel,将回调Acceptor：：newconnection(),如果是
  std::function<void()>closecallback_;  //关闭fd_的回调函数,将回调Connection::closecallback()
  std::function<void()>errorcallback_;  //fd_发生了错误的回调函数，将回调Connection::errorcallback()
  std::function<void()>writecallback_;   //fd_写事件的回调函数，将回调Connection::errorcallback()
  
public:
  Channel(EventLoop* loop_, int fd);  //构造函数，Channel是Acceptor和Connection的下层类
 ~Channel();
 
 int fd();                        //返回fd_成员
 void useET();                    //采用边缘触发
 void enableReading();            //让epoll_wait()监听fd_的读事件，注册读事件
 void disableReading();           //取消读事件
 void enableWriting();            //注册写事件
 void disableWriting();           //取消写事件
 void disableall();               //取消全部的事件
 void remove();                   //从事件循环中删除Channel
 void setinepoll(bool inepoll);  //设置inepoll_成员的值设置为true
 void setrevents(uint32_t ev);   //设置revents_成员的值为参数ev
 bool inpoll();       //返回inepoll_成员
 uint32_t events();   //返回events_成员
 uint32_t revents();  //返回revents_成员
 
 void handleEvent();    //事件处理函数，epoll_wait()返回的时候，执行它

 void setreadcallback(std::function<void()>fn); //设置fd_读事件的回调函数
 void setclosecallback(std::function<void()>fn);  //关闭fd_的回调函数,将回调Connection::closecallback()
 void seterrorcallback(std::function<void()>fn);  //fd_发生了错误的回调函数，将回调Connection::errorcallback()
 void setwritecallback(std::function<void()>fn);   //设置写事件的回调函数
 
};