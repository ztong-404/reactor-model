#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<strings.h>
#include<string.h>
#include<sys/epoll.h>
#include<vector>
#include<unistd.h>
#include"Channel.h"
//checked
class Channel;

/*****************************************
Epoll��
����Ҫ���ӵ��¼�����ӵ�������ϻ�Ӻ������ɾ��channel

******************************************/
class Epoll
{
private:
  static const int MaxEvents=100; //epoll_wait()�����¼�����Ĵ�С
  int epollfd_=-1;                 //epoll���
  epoll_event events_[MaxEvents]; //���epoll_wait()�����¼������飬�ڹ��캯���з����ڴ�
public:
  Epoll();
  ~Epoll();
  
  void updatechannel(Channel *ch);     //��channel���/���µ�������ϣ�channel����fd��Ҳ����Ҫ���ӵ��¼�
  void removechannel(Channel *ch);      //�Ӻ������ɾ��channel
  std::vector<Channel*>loop(int timeout=-1);    //����epoll_wait()���ȴ��¼��ķ������ѷ������¼���vector��������

};