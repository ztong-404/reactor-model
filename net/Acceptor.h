#pragma once

#include"Socket.h"
#include"InetAddress.h"
#include"Channel.h"
#include"EventLoop.h"

#include<functional>
#include<memory>

/*******************************
��װ�˷�������ڼ�����socket�࣬�����¿ͻ�����������
********************************/

class Acceptor
{
private:
  EventLoop* loop_;    //Acceptor��Ӧ���¼�ѭ�����ڹ��캯���д���
  Socket servsock_;   //��������ڼ�����socket���ڹ��캯���д���
  Channel acceptchannel_;  //Acceptor��Ӧ��channel���ڹ��캯���д���
  std::function<void(std::unique_ptr<Socket>)>newconnectioncb_;//�����¿ͻ�����������Ļص���������ָ��TcpServer:

public:
  Acceptor(EventLoop* loop,const std::string &ip,const uint16_t port);
  ~Acceptor();
  
  void newConnection(); //�����¿ͻ�����������
  
  void setnewconnectioncb(std::function<void(std::unique_ptr<Socket>)>fn); //���ô����¿ͻ�����������Ļص�����
};