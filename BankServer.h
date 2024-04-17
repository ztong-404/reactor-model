#pragma once
#include"TcpServer.h"
#include"EventLoop.h"
#include"Connection.h"
#include"ThreadPool.h"
//checked
/*
  BankServer�ࣺ�������з�����
*/
class UserInfo //�û���Ϣ(״̬��)
{
private:
  int fd_;           //�ͻ��˵�fd
  std::string ip_;   //�ͻ��˵�ip��ַ
  bool login_=false; //�ͻ��˵�¼��״̬
  std::string name_; //�ͻ��˵��û���
  
public:
  UserInfo(int fd,const std::string &ip):fd_(fd),ip_(ip){}
  void setLoin(bool login){login_=login;}
  bool Login(){return login_;}  
}


class BankServer
{
private:
  using spUserInfo=std::shared_ptr<UserInfo>; 

  TcpServer tcpserver_;
  ThreadPool threadpool_;                     //�����̳߳�
  std::mutex mutex_;                           //����usermap_�Ļ�����
  std::map<int,spUserInfo>usermap_;           //�û���״̬��

public:
  BankServer(const std::string &ip,const uint16_t port,int subthreadnum=3,int workthreadnum=5);
  ~BankServer();
  
  void Start();  //��������
  void Stop();   //ֹͣ����
  
  void HandleNewConnection(spConnection conn);  //�����¿ͻ�������������TcpServer���лص��˺���
  void HandleClose(spConnection conn);  //�رտͻ��˵����ӣ���TcpServer���лص��˺���
  void HandleError(spConnection conn);  //�ͻ��˵����Ӵ�����TcpServer���лص��˺���
  void HandleMessage(spConnection conn,std::string& message);  //����ͻ��˵����ʱ��ģ���TcpServer���лص��˺���
  void HandleSendComplete(spConnection conn);     //���ݷ�����ɺ���TcpServer���лص��˺���

  void OnMessage(spConnection conn,std::string &message);     //����ͻ��˵������ģ�������Ӹ��̳߳�
  void HandleRemove(int fd);
};