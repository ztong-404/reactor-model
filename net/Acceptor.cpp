#include"Acceptor.h"

Acceptor::Acceptor(EventLoop* loop,const std::string &ip,const uint16_t port)
          :loop_(loop),servsock_(createnonblocking()),acceptchannel_(loop_,servsock_.fd())
{
  InetAddress servaddr(ip,port);     //����˵ĵ�ַ��Э��
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
//�����¿ͻ�����������
void Acceptor::newConnection()
{
  InetAddress clientaddr;
       
  //newһ���µ�fd����������ͻ��˵�ͨ��
  std::unique_ptr<Socket> clientsock(new Socket(servsock_.accept(clientaddr)));
  clientsock->setIpPort(clientaddr.ip(),clientaddr.port());
       
  newconnectioncb_(std::move(clientsock)); // �ص�TcpServer::newconnection()
}

void Acceptor::setnewconnectioncb(std::function<void(std::unique_ptr<Socket>)>fn) //���ô����¿ͻ�����������Ļص�����
{
  newconnectioncb_=fn;
}