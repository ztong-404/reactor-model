#include"Channel.h"
 
 Channel::Channel(EventLoop* loop, int fd):fd_(fd),loop_(loop)
 {}
 
 Channel::~Channel()
 {}
 
 int Channel::fd()
 {
   return fd_;
 }
                          
 void Channel::useET()  //采用边缘触发
 {
   events_=events_|EPOLLET;
 }
                     
 //让epoll_wait()监听fd_的读事件，注册读事件
 void Channel::enableReading() 
 {
   events_|=EPOLLIN;  
   loop_->updatechannel(this);
 }   
      
  //取消读事件
 void Channel::disableReading()
 {
   events_&=~EPOLLIN;
   loop_->updatechannel(this);
 }
 
  //注册写事件          
 void Channel::enableWriting()
 {
   events_|=EPOLLOUT;
   loop_->updatechannel(this);
 }  
 
 //取消写事件           
 void Channel::disableWriting()
 {
   events_&=~EPOLLOUT;
   loop_->updatechannel(this);
 }       
 
 //取消全部的事件
 void Channel::disableall()
 {
   events_=0;
   loop_->updatechannel(this);
 }
           
 //从事件循环中删除Channel    
 void Channel::remove()            
 {
   disableall();                     //先取消全部的事件
   loop_->removechannel(this);       //从红黑树上删除fd
 }
 
 
 void Channel::setinepoll(bool inepoll)
 {
   inepoll_=inepoll;
 }
       
 //设置revents_成员的值为参数ev
 void Channel::setrevents(uint32_t ev)  
 {
   revents_=ev;
 }
 
 bool Channel::inpoll()       
 {
   return inepoll_;
 }
 
 uint32_t Channel::events()   
 {
   return events_;
 }
 
 uint32_t Channel::revents()
 {
   return revents_;
 }
 
 //事件处理函数，epoll_wait()返回的时候，执行它
void Channel::handleEvent() 
{
   if(revents_&EPOLLRDHUP)
   {
     closecallback_();
   }
   else if(revents_&(EPOLLIN|EPOLLPRI))  //接受缓冲区中有数据可以读
   {
     readcallback_();                    //如果是acceptchannel，回调Acceptor：：newconnection(),如果是clientchannel，回调Channel：：onmessage()
   }
   else if(revents_&EPOLLOUT)
   {    
     writecallback_();                   //回调Connection：：writecallback()
   }
   else                                  //其他事件都视为错位
   {                       
     errorcallback_();
   }
}
 
 //设置fd_读事件的回调函数 
void Channel::setreadcallback(std::function<void()>fn)
{
  readcallback_=fn;
}

 //设置关闭fd_的回调函数 
void Channel::setclosecallback(std::function<void()>fn)
{
  closecallback_=fn;
}

 //设置fd_发生了错误的回调函数 
void Channel::seterrorcallback(std::function<void()>fn)
{
  errorcallback_=fn;
} 

//设置写事件的回调函数
void Channel::setwritecallback(std::function<void()>fn)
{
  writecallback_=fn;
}
 
 
 
 
 
 