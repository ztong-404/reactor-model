#include"TcpServer.h"

TcpServer::TcpServer(const std::string &ip, const uint16_t port, int threadnum)
          :threadnum_(threadnum),mainloop_(new EventLoop(true)),
           acceptor_(mainloop_.get(),ip,port),threadpool_(threadnum_,"IO")
{
  //�������¼�ѭ��
  mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
  
  acceptor_.setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
  
  //�������¼�ѭ��
  for(int ii=0;ii<threadnum_;ii++)
  {
    subloops_.emplace_back(new EventLoop(false,5,10));  //�������¼�ѭ��������subloops_������
    subloops_[ii]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));//����timeout��ʱ�Ļص�
    subloops_[ii]->settimercallback(std::bind(&TcpServer::removeconn,this,std::placeholders::_1));//�����������TCP���ӵĻص�����
    threadpool_.addtask(std::bind(&EventLoop::run,subloops_[ii].get()));  //���̳߳������д��¼�ѭ��
  }
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
  mainloop_->run();
}

//ֹͣIO�̺߳��¼�ѭ��
void TcpServer::stop()
{
  //ֹͣ���¼�ѭ��
  mainloop_->stop();
  printf("mainloop thread already stop.\n");
  
  //ֹͣ���¼�ѭ��
  for(int ii=0;ii<threadnum_;ii++)
  {
    subloops_[ii]->stop();
  }
  printf("subloop thread already stop.\n");
 
  //ֹͣIO�߳�
  threadpool_.stop();
  printf("IO threadpool already stop.\n");
}

//�����¿ͻ�����������
void TcpServer::newconnection(std::unique_ptr<Socket> clientsock)
{
  //���½���conn��������¼�ѭ��
  spConnection conn(new Connection(subloops_[clientsock->fd()%threadnum_].get(),std::move(clientsock)));  
  conn->setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
  conn->seterrorcallback(std::bind(&TcpServer::errorconnection,this,std::placeholders::_1));
  conn->setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
  conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete,this,std::placeholders::_1));
 
  {
    std::lock_guard<std::mutex>gd(mmutex_);
    conns_[conn->fd()]=conn; //��conn�����map������
  }
  subloops_[conn->fd()%threadnum_]->newconnection(conn);  //��conn��ŵ�EventLoop��map������
  
  if(newconnectioncb_)newconnectioncb_(conn);   //�ص�EchoServer::HandleNewConnection().
}

//�رտͻ��˵����ӣ���Connection���лص��˺���
void TcpServer::closeconnection(spConnection conn)
{
  if (closeconnectioncb_) closeconnectioncb_(conn);
  
  {
    std::lock_guard<std::mutex>gd(mmutex_);
    conns_.erase(conn->fd());
  }
}

//�ͻ��˵����Ӵ�����Connection���лص��˺���
void TcpServer::errorconnection(spConnection conn)
{
  if (errorconnectioncb_) errorconnectioncb_(conn); //�ص�EchoServer::HandleError().
  
  {
    std::lock_guard<std::mutex>gd(mmutex_);
    conns_.erase(conn->fd()); //��map��ɾ��conn
  }
}

//����ͻ��˵����ʱ��ģ���Connection���лص��˺���
void TcpServer::onmessage(spConnection conn, std::string& message)
{
  if (onmessagecb_) onmessagecb_(conn,message); //�ص�EchoServer::HandleMessage().
}

//���ݷ�����ɺ���Connection���лص��˺���
void TcpServer::sendcomplete(spConnection conn)
{
 // printf("send Complete.\n");
  
  if (sendcompletecb_) sendcompletecb_(conn); //�ص�EchoServer::HandleSendComplete().
}

 //epoll_wait()��ʱ����EventLoop���лص��˺���
void TcpServer::epolltimeout(EventLoop *loop)
{

  if (timeoutcb_) timeoutcb_(loop); //�ص�EchoServer����HandleTimeout().
}

 //�����¿ͻ�����������
void TcpServer::setnewconnectioncb(std::function<void(spConnection)>fn)
{
  newconnectioncb_=fn;
}  

 //�رտͻ��˵����ӣ���Connection���лص��˺���
void TcpServer::setcloseconnectioncb(std::function<void(spConnection)>fn)
{
  closeconnectioncb_=fn;
} 

//�ͻ��˵����Ӵ�����Connection���лص��˺���
void TcpServer::seterrorconnectioncb(std::function<void(spConnection)>fn)
{
  errorconnectioncb_=fn;
}

//����ͻ��˵����ʱ��ģ���Connection���лص��˺���
void TcpServer::setonmessagecb(std::function<void(spConnection,std::string &message)>fn)
{
  onmessagecb_=fn;
}

//���ݷ�����ɺ���Connection���лص��˺���
void TcpServer::setsendcompletecb(std::function<void(spConnection)>fn)
{
  sendcompletecb_=fn;
}

//epoll_wait()��ʱ����EventLoop���лص��˺���
void TcpServer::settimeoutcb(std::function<void(EventLoop*)>fn)
{
  timeoutcb_=fn;
}

//ɾ��conns_�е�Connection������EventLoop::handletimer()�н��ص��˺���
void TcpServer::removeconn(int fd)
{
  {
    std::lock_guard<std::mutex>gd(mmutex_);
    conns_.erase(fd); //��map��ɾ��conn
  }
  if(removeconnectioncb_)removeconnectioncb_(fd);
}

void TcpServer::setremoveconnectioncb(std::function<void(int)>fn)
{
  removeconnectioncb_=fn;
}





