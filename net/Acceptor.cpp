#include"Acceptor.h"

Acceptor::Acceptor(EventLoop* loop,const std::string &ip,const uint16_t port)
          :loop_(loop),servsock_(createnonblocking()),acceptchannel_(loop_,servsock_.fd())
{
  InetAddress servaddr(ip,port);     //服务端的地址和协议
  servsock_.setReuseaddr(true);
  servsock_.setTcpnodelay(true);
  servsock_.setReuseport(true);
  servsock_.setKeepalive(true);
  servsock_.bind(servaddr);
  servsock_.listen();
  
  acceptchannel_.setreadcallback(std::bind(&Acceptor::newConnection,this));
  acceptchannel_.enableReading();
}

Acceptor::~Acceptor()
{
}


#include"Connection.h"
//处理新客户端连接请求
void Acceptor::newConnection()
{
  InetAddress clientaddr;
       
  //new一个新的fd用来处理与客户端的通信
  std::unique_ptr<Socket> clientsock(new Socket(servsock_.accept(clientaddr)));
  clientsock->setIpPort(clientaddr.ip(),clientaddr.port());
       
  newconnectioncb_(std::move(clientsock)); // 回调TcpServer::newconnection()
}

void Acceptor::setnewconnectioncb(std::function<void(std::unique_ptr<Socket>)>fn) //设置处理新客户端连接请求的回调函数
{
  newconnectioncb_=fn;
}