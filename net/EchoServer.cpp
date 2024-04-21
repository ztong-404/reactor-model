#include"EchoServer.h"

EchoServer::EchoServer(const std::string &ip,const uint16_t port, int subthreadnum, int workthreadnum)
          :tcpserver_(ip,port,subthreadnum),threadpool_(workthreadnum,"WORKS")
{
  tcpserver_.setnewconnectioncb(std::bind(&EchoServer::HandleNewConnection,this,std::placeholders::_1));
  tcpserver_.setcloseconnectioncb(std::bind(&EchoServer::HandleClose,this,std::placeholders::_1));
  tcpserver_.seterrorconnectioncb(std::bind(&EchoServer::HandleError,this,std::placeholders::_1));
  tcpserver_.setonmessagecb(std::bind(&EchoServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2));
  tcpserver_.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete,this,std::placeholders::_1));
}

EchoServer::~EchoServer()
{}

//��������
void EchoServer::Start()
{
  tcpserver_.start();
}

//ֹͣ����
void EchoServer::Stop()
{
  //ֹͣ�����߳�
  threadpool_.stop();
  printf("thread working stop.\n");
  //ֹͣIO�̣߳��¼�ѭ����
  tcpserver_.stop();
}
  
 //�����¿ͻ�������������TcpServer���лص��˺���  
void EchoServer::HandleNewConnection(spConnection conn)
{

   printf("new connection(fd=%d, ip=%s, port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port()); 
} 

//�رտͻ��˵����ӣ���TcpServer���лص��˺���
void EchoServer::HandleClose(spConnection conn)
{
  printf("connection close.(fd=%d, ip=%s, port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port()); 
}

//�ͻ��˵����Ӵ�����TcpServer���лص��˺���
void EchoServer::HandleError(spConnection conn)
{
   std::cout<<"EchoServer conn error"<<std::endl;
} 

//����ͻ��˵����ʱ��ģ���TcpServer���лص��˺���
void EchoServer::HandleMessage(spConnection conn,std::string& message)
{

   if(threadpool_.size()==0)
   {
     //���û�й����̣߳���ʾ��IO�߳��м���
     OnMessage(conn,message);
   } 
   else
   {
     //��ҵ����ӵ��̳߳ص����������
     threadpool_.addtask(std::bind(&EchoServer::OnMessage,this,conn,message));
   }
}  

//����ͻ��˵������ģ�������Ӹ��̳߳�
void EchoServer::OnMessage(spConnection conn, std::string &message) 
{
  //��������������ɲ��������
   message="reply:"+message;  //����ҵ��
   conn->send(message.data(),message.size()); //�����ݷ��ͳ�ȥ
}

 //���ݷ�����ɺ���TcpServer���лص��˺���
void EchoServer::HandleSendComplete(spConnection conn)
{
  std::cout<<"Message send complete."<<std::endl;
}    
