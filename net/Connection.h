#pragma once

#include<sys/syscall.h>
#include<functional>
#include<memory>
#include<atomic>

#include"Socket.h"
#include"EventLoop.h"
#include"InetAddress.h"
#include"Channel.h"
#include"EventLoop.h"
#include"Buffer.h"
#include"Timestamp.h"

/**********************************************
��װ�˷����������ͻ���ͨѶ��socket�࣬������ͻ��˴���������Ϣ

***********************************************/

class EventLoop;
class Channel;
class Connection;
using spConnection=std::shared_ptr<Connection>;

/*��װ�˷��������ͨ�ŵ�channel*/
class Connection:public std::enable_shared_from_this<Connection>
{
private:
  EventLoop* loop_;    //Connection��Ӧ���¼�ѭ�����ڹ��캯���д���
  std::unique_ptr<Socket> clientsock_;   //��ͻ���ͨѶ��Socket
  std::unique_ptr<Channel>clientchannel_;  //Connection��Ӧ��channel���ڹ��캯���д���
  Buffer inputBuffer_;     //���ջ�����
  Buffer outputBuffer_;    //���ͻ�����
  std::atomic_bool disconnect_;   //�ͻ��������Ƿ��ѶϿ�������ѶϿ���������Ϊtrue
  
  std::function<void(spConnection)>closecallback_;  //�ر�fd_�Ļص����������ص�TcpServer����closeconnection().
  std::function<void(spConnection)>errorcallback_;  //fd_�����˴���Ļص����������ص�TcpServer����errorconnection().
  std::function<void(spConnection,std::string&)>onmessagecallback_; //�����ĵĻص����������ص�TcpServer::onmessage().
  std::function<void(spConnection)>sendcompletecallback_;  //�����ĵĻص����������ص�TcpServer����onmessage().
  Timestamp lastatime_;                                    //ʱ���,����Connection����ʱΪ��ǰʱ�䣬ÿ���ܵ�һ�����ģ���ʱ�������Ϊ��ǰʱ��
  

public:
  Connection(EventLoop* loop,std::unique_ptr<Socket> clientsock);
  ~Connection();
  
  int fd()const;          //���ؿͻ��˵�fd_
  std::string ip()const;  //���ؿͻ��˵�ip
  uint16_t port()const;   //���ؿͻ��˵�port
  
  void onMessage();       //����Զ˷��͹�������Ϣ
  void closecallback();   //TCP���ӹر�(�Ͽ�)�Ļص���������Channel�ص�
  void errorcallback();   //TCP���Ӵ���Ļص���������Channel�ص�
  void writecallback();   //����д�¼��Ļص���������Channel�ص�
  
  void setclosecallback(std::function<void(spConnection)>fn); //���ùر�fd_�Ļص�����
  void seterrorcallback(std::function<void(spConnection)>fn); //����fd_�����˴���Ļص�����
  void setonmessagecallback(std::function<void(spConnection,std::string&)>fn); //���ô����ĵĻص�����
  void setsendcompletecallback(std::function<void(spConnection)>fn); //����������ɺ�Ļص�����
  
  void send(const char *data, size_t size);  //��������,�������κ��߳��У����ǵ��ô˺�����������
  void sendinloop(const char *data, size_t size);   //�����IO�̣߳�ֱ�ӵ��ô˺���������ǹ����̣߳����Ѵ˺�������IO�߳�
  
  bool timeout(time_t now,int val);  //�ж�TCP�����Ƿ�ʱ,(����̫��)
};
