 #pragma once

#include<sys/epoll.h>
#include<functional>
#include<memory>

#include"EventLoop.h"
#include"InetAddress.h"
#include"Socket.h"
#include"Epoll.h"

/************************
Channel��

************************/


class EventLoop;

class Channel
{
private:
  int fd_=-1;                  //channel ӵ�е�fd�� channel��fd��һ��һ�Ĺ�ϵ
  EventLoop* loop_;       //Channel��Ӧ���¼�ѭ����Channel��EventLoop�Ƕ��һ�Ĺ�ϵ��һ��channelֻ��Ӧһ��EventLoop
  bool inepoll_=false;         //channel�Ƿ�����ӵ�epoll���ϣ����δ��ӣ�����epoll_ctl()��EPOLL_CTL_ADD,������EPOLL_CTL_MOD
  uint32_t events_=0;         //fd_��Ҫ���ӵ��¼�. listenfd��clientfd��Ҫ����EPOLLIN,clientfd������Ҫ����EPOLLOUT
  uint32_t revents_=0;       //fd_�ѷ������¼�
  std::function<void()>readcallback_;  //fd_���¼��Ļص������� �����acceptchannel,���ص�Acceptor����newconnection(),�����
  std::function<void()>closecallback_;  //�ر�fd_�Ļص�����,���ص�Connection::closecallback()
  std::function<void()>errorcallback_;  //fd_�����˴���Ļص����������ص�Connection::errorcallback()
  std::function<void()>writecallback_;   //fd_д�¼��Ļص����������ص�Connection::errorcallback()
  
public:
  Channel(EventLoop* loop_, int fd);  //���캯����Channel��Acceptor��Connection���²���
 ~Channel();
 
 int fd();                        //����fd_��Ա
 void useET();                    //���ñ�Ե����
 void enableReading();            //��epoll_wait()����fd_�Ķ��¼���ע����¼�
 void disableReading();           //ȡ�����¼�
 void enableWriting();            //ע��д�¼�
 void disableWriting();           //ȡ��д�¼�
 void disableall();               //ȡ��ȫ�����¼�
 void remove();                   //���¼�ѭ����ɾ��Channel
 void setinepoll(bool inepoll);  //����inepoll_��Ա��ֵ����Ϊtrue
 void setrevents(uint32_t ev);   //����revents_��Ա��ֵΪ����ev
 bool inpoll();       //����inepoll_��Ա
 uint32_t events();   //����events_��Ա
 uint32_t revents();  //����revents_��Ա
 
 void handleEvent();    //�¼���������epoll_wait()���ص�ʱ��ִ����

 void setreadcallback(std::function<void()>fn); //����fd_���¼��Ļص�����
 void setclosecallback(std::function<void()>fn);  //�ر�fd_�Ļص�����,���ص�Connection::closecallback()
 void seterrorcallback(std::function<void()>fn);  //fd_�����˴���Ļص����������ص�Connection::errorcallback()
 void setwritecallback(std::function<void()>fn);   //����д�¼��Ļص�����
 
};