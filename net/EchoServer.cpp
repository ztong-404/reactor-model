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

//启动服务
void EchoServer::Start()
{
  tcpserver_.start();
}

//停止服务
void EchoServer::Stop()
{
  //停止工作线程
  threadpool_.stop();
  printf("thread working stop.\n");
  //停止IO线程（事件循环）
  tcpserver_.stop();
}
  
 //处理新客户端连接请求，在TcpServer类中回调此函数  
void EchoServer::HandleNewConnection(spConnection conn)
{

   printf("new connection(fd=%d, ip=%s, port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port()); 
} 

//关闭客户端的连接，在TcpServer类中回调此函数
void EchoServer::HandleClose(spConnection conn)
{
  printf("connection close.(fd=%d, ip=%s, port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port()); 
}

//客户端的连接错误，在TcpServer类中回调此函数
void EchoServer::HandleError(spConnection conn)
{
   std::cout<<"EchoServer conn error"<<std::endl;
} 

//处理客户端的请问报文，在TcpServer类中回调此函数
void EchoServer::HandleMessage(spConnection conn,std::string& message)
{

   if(threadpool_.size()==0)
   {
     //如果没有工作线程，表示在IO线程中计算
     OnMessage(conn,message);
   } 
   else
   {
     //把业务添加到线程池的任务队列中
     threadpool_.addtask(std::bind(&EchoServer::OnMessage,this,conn,message));
   }
}  

//处理客户端的请求报文，用于添加给线程池
void EchoServer::OnMessage(spConnection conn, std::string &message) 
{
  //在这里，将经过若干步骤的运算
   message="reply:"+message;  //回显业务
   conn->send(message.data(),message.size()); //把数据发送出去
}

 //数据发送完成后，在TcpServer类中回调此函数
void EchoServer::HandleSendComplete(spConnection conn)
{
  std::cout<<"Message send complete."<<std::endl;
}    
