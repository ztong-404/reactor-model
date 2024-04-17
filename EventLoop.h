#pragma once

#include<functional>
#include<memory>
#include<unistd.h>
#include<queue>
#include<map>
#include<mutex>
#include<atomic>
#include<functional>
#include<sys/eventfd.h>
#include<sys/syscall.h>
#include<sys/timerfd.h>

#include"Connection.h"
#include"Epoll.h"

//checked
/*
�¼�ѭ����


*/
class Channel;
class Epoll;
class Connection;
using spConnection=std::shared_ptr<Connection>;

//�¼�ѭ����
class EventLoop
{
private:
  int timetvl_;                                     //����ʱ��������λ����
  int timeout_;                                     //Connection����ʱ��ʱ�䣬��λ����
  int wakeupfd_;                                    //���ڻ����¼�ѭ���̵߳�eventfd
  int timerfd_;                                     //��ʱ����fd
  bool mainloop_;                                   //true�����¼�ѭ����false�Ǵ��¼�ѭ��
  std::atomic_bool stop_;                           //��ʼֵΪfalse, �������Ϊtrue����ʾֹͣ�¼�ѭ��
  pid_t threadid_;                                  //�¼�ѭ�������̵߳�id
  std::unique_ptr<Epoll>ep_;                       //ÿ���¼�ѭ��ֻ��һ��Epoll
  std::unique_ptr<Channel> wakechannel_;           //eventfd��Channel                         
  std::unique_ptr<Channel>timerchannel_;            //��ʱ����channel
  std::mutex mutex_;                               //�������ͬ���Ļ�����
  std::mutex mmutex_;                                //����conns_�Ļ�����
  
  std::queue<std::function<void()>>taskqueue_;     //�¼�ѭ���̱߳�eventfd���Ѻ�ִ�е��������
  std::map<int,spConnection>conns_;                 //��������ڸ��¼�ѭ����ȫ����Connection���� 
  std::function<void(int)>timercallback_;              //ɾ��TcpServer�г�ʱ��Connection���󣬽�������ΪTcpServer����removeconn()
  std::function<void(EventLoop*)>epolltimeoutcallback_;   //epoll_wait()��ʱ�Ļص�����
 
public:
  EventLoop(bool mainloop, int timetvl=30, int timeout=80);  //���캯���д���epoll����
  ~EventLoop(); //��������������epoll����

  void run(); //�����¼�ѭ��
  void stop(); //ֹͣ�¼�ѭ��
  
  void updatechannel(Channel *ch);  //��channel���/���µ�������ϣ�channel����fd��Ҳ����Ҫ���ӵ��¼�
  void removechannel(Channel *ch);  //�Ӻ������ɾ��channel
  void setepolltimeoutcallback(std::function<void(EventLoop*)>fn); //����eoll_wait������ʱ�Ļص�����
  
  bool isinloopthread();                       //�жϵ�ǰ�߳��Ƿ�Ϊ�¼�ѭ���̡߳�

  void queueinloop(std::function<void()>fn);   //��������ӵ�������
  void wakeup();                               //��eventfd�����¼�ѭ���߳�   
  void handlewakeup();                         //�¼�ѭ���̱߳�evenfd���Ѻ�ִ�еĺ��� 
  void handletimer();                          //������ʱִ�еĺ���
  
  void newconnection(spConnection conn);       //��Connection���󱣴���conns_��
  void settimercallback(std::function<void(int)>fn);   //��������ΪTcpServer::removeconn()
};