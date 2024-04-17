#include"Channel.h"
 
 Channel::Channel(EventLoop* loop, int fd):fd_(fd),loop_(loop)
 {}
 
 Channel::~Channel()
 {}
 
 int Channel::fd()
 {
   return fd_;
 }
                          
 void Channel::useET()  //���ñ�Ե����
 {
   events_=events_|EPOLLET;
 }
                     
 //��epoll_wait()����fd_�Ķ��¼���ע����¼�
 void Channel::enableReading() 
 {
   events_|=EPOLLIN;
   loop_->updatechannel(this);
 }   
      
  //ȡ�����¼�
 void Channel::disableReading()
 {
   events_&=~EPOLLIN;
   loop_->updatechannel(this);
 }
 
  //ע��д�¼�          
 void Channel::enableWriting()
 {
   events_|=EPOLLOUT;
   loop_->updatechannel(this);
 }  
 
 //ȡ��д�¼�           
 void Channel::disableWriting()
 {
   events_&=~EPOLLOUT;
   loop_->updatechannel(this);
 }       
 
 //ȡ��ȫ�����¼�
 void Channel::disableall()
 {
   events_=0;
   loop_->updatechannel(this);
 }
           
 //���¼�ѭ����ɾ��Channel    
 void Channel::remove()            
 {
   disableall();                     //��ȡ��ȫ�����¼�
   loop_->removechannel(this);       //�Ӻ������ɾ��fd
 }
 
 
 void Channel::setinepoll(bool inepoll)
 {
   inepoll_=inepoll;
 }
       
 //����revents_��Ա��ֵΪ����ev
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
 
 //�¼���������epoll_wait()���ص�ʱ��ִ����
void Channel::handleEvent() 
{
   if(revents_&EPOLLRDHUP)
   {
     closecallback_();
   }
   else if(revents_&(EPOLLIN|EPOLLPRI))  //���ܻ������������ݿ��Զ�
   {
     readcallback_();                    //�����acceptchannel���ص�Acceptor����newconnection(),�����clientchannel���ص�Channel����onmessage()
   }
   else if(revents_&EPOLLOUT)
   {    
     writecallback_();                   //�ص�Connection����writecallback()
   }
   else                                  //�����¼�����Ϊ��λ
   {                       
     errorcallback_();
   }
}
 
 //����fd_���¼��Ļص����� 
void Channel::setreadcallback(std::function<void()>fn)
{
  readcallback_=fn;
}

 //���ùر�fd_�Ļص����� 
void Channel::setclosecallback(std::function<void()>fn)
{
  closecallback_=fn;
}

 //����fd_�����˴���Ļص����� 
void Channel::seterrorcallback(std::function<void()>fn)
{
  errorcallback_=fn;
} 

//����д�¼��Ļص�����
void Channel::setwritecallback(std::function<void()>fn)
{
  writecallback_=fn;
}
 
 
 
 
 
 