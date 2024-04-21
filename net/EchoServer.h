#pragma once
#include"TcpServer.h"
#include"EventLoop.h"
#include"Connection.h"
#include"ThreadPool.h"
//checked
/*
  EchoServer�ࣺ���Է�����
*/

class EchoServer
{
private:
  TcpServer tcpserver_;
  ThreadPool threadpool_;   //�����̳߳�

public:
  EchoServer(const std::string &ip,const uint16_t port,int subthreadnum=3,int workthreadnum=5);
  ~EchoServer();
  
  void Start();  //��������
  void Stop();   //ֹͣ����
  
  void HandleNewConnection(spConnection conn);  //�����¿ͻ�������������TcpServer���лص��˺���
  void HandleClose(spConnection conn);  //�رտͻ��˵����ӣ���TcpServer���лص��˺���
  void HandleError(spConnection conn);  //�ͻ��˵����Ӵ�����TcpServer���лص��˺���
  void HandleMessage(spConnection conn,std::string& message);  //����ͻ��˵����ʱ��ģ���TcpServer���лص��˺���
  void HandleSendComplete(spConnection conn);     //���ݷ�����ɺ���TcpServer���лص��˺���

  void OnMessage(spConnection conn,std::string &message);     //����ͻ��˵������ģ�������Ӹ��̳߳�
   
};