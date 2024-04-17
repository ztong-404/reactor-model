#pragma once

#include"EventLoop.h"
#include"Socket.h"
#include"Channel.h"
#include"Acceptor.h"
#include"Connection.h"
#include"ThreadPool.h"

#include<map>
#include<mutex>
#include<functional>
#include<memory>
//checked
//TCP���������
class TcpServer
{
private:
  std::unique_ptr<EventLoop>mainloop_; //���¼�ѭ��
  std::vector<std::unique_ptr<EventLoop>>subloops_;   //��Ŵ��¼�ѭ��������
  Acceptor acceptor_;     //һ��TcpServerֻ��һ��Acceptor����
  int threadnum_;               //�̳߳صĴ�С,�����¼�ѭ���ĸ���
  ThreadPool threadpool_;      //�̳߳�
  std::mutex mmutex_;          //����conns_�Ļ�����
  std::map<int,spConnection>conns_;  //һ��TcpServer�ж��Connection����, �����map������
  
  std::function<void(spConnection)>newconnectioncb_;
  std::function<void(spConnection)>closeconnectioncb_;
  std::function<void(spConnection)>errorconnectioncb_;
  std::function<void(spConnection,std::string &message)>onmessagecb_;
  std::function<void(spConnection)>sendcompletecb_;
  std::function<void(EventLoop*)>timeoutcb_;
  std::function<void(int)>removeconnectioncb_;      //�ص�BackServer::HandleRemove();
  
public:
  TcpServer(const std::string &ip, const uint16_t port, int threadnum_=3);
  ~TcpServer();

  void start(); //�����¼�ѭ��
  void stop(); //ֹͣIO�̺߳��¼�ѭ��
  
  void newconnection(std::unique_ptr<Socket> clientsock);  //�����¿ͻ�����������
  void closeconnection(spConnection conn);  //�رտͻ��˵����ӣ���Connection���лص��˺���
  void errorconnection(spConnection conn);  //�ͻ��˵����Ӵ�����Connection���лص��˺���
  void onmessage(spConnection conn,std::string& message);  //����ͻ��˵����ʱ��ģ���Connection���лص��˺���
  void sendcomplete(spConnection conn);     //���ݷ�����ɺ���Connection���лص��˺���
  void epolltimeout(EventLoop *loop);    //epoll_wait()��ʱ����EventLoop���лص��˺���
  
  void setnewconnectioncb(std::function<void(spConnection)>fn);  //�����¿ͻ�����������
  void setcloseconnectioncb(std::function<void(spConnection)>fn);  //�رտͻ��˵����ӣ���Connection���лص��˺���
  void seterrorconnectioncb(std::function<void(spConnection)>fn);  //�ͻ��˵����Ӵ�����Connection���лص��˺���
  void setonmessagecb(std::function<void(spConnection,std::string &message)>fn);  //����ͻ��˵����ʱ��ģ���Connection���лص��˺���
  void setsendcompletecb(std::function<void(spConnection)>fn);     //���ݷ�����ɺ���Connection���лص��˺���
  void settimeoutcb(std::function<void(EventLoop*)>fn);    //epoll_wait()��ʱ����EventLoop���лص��˺���
  
  void removeconn(int fd);   //ɾ��conns_�е�Connection������EventLoop::handletimer()�н��ص��˺���
  void setremoveconnectioncb(std::function<void(int)>fn);
};