#include"Connection.h"

Connection::Connection(EventLoop* loop,std::unique_ptr<Socket> clientsock)
            :loop_(loop),clientsock_(std::move(clientsock)),disconnect_(false),clientchannel_(new Channel(loop_,clientsock_->fd()))
{
  //Ϊ�¿ͻ���׼�����¼�������ӵ�epoll��
  clientchannel_->setreadcallback(std::bind(&Connection::onMessage,this));
  clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
  clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
  clientchannel_->setwritecallback(std::bind(&Connection::writecallback,this));
  clientchannel_->useET();  //�ͻ�������������fd���ñ�Ե����
  clientchannel_->enableReading();  //��epoll_wait()����clientchannel�Ķ��¼�
}

Connection::~Connection()
{
}

int Connection::fd()const    
{
  return clientsock_->fd();
}

std::string Connection::ip()const
{
  return clientsock_->ip();
}

uint16_t Connection::port()const
{
  return clientsock_->port();
}

void Connection::closecallback()
{
  disconnect_=true;
  clientchannel_->remove();    //���¼�ѭ����ɾ��Channel
  closecallback_(shared_from_this());
}

void Connection::errorcallback()
{
  disconnect_=true;
  clientchannel_->remove();    //���¼�ѭ����ɾ��Channel
  errorcallback_(shared_from_this());
}


void Connection::setclosecallback(std::function<void(spConnection)>fn) 
{
  closecallback_=fn;
}

void Connection::seterrorcallback(std::function<void(spConnection)>fn)
{
  errorcallback_=fn;
}

void Connection::setonmessagecallback(std::function<void(spConnection,std::string&)>fn)
{
  onmessagecallback_=fn;
}

void Connection::setsendcompletecallback(std::function<void(spConnection)>fn)
{
  sendcompletecallback_=fn;
}


//����Զ˷��͹�������Ϣ
void Connection::onMessage()
{
  char buffer[1024];
  while(true)
  {
    bzero(&buffer,sizeof(buffer));
    ssize_t nread=read(fd(),buffer,sizeof(buffer)); 
         
    if(nread>0)
    {
      inputBuffer_.append(buffer,nread);      
    }
    else if(nread==-1&&errno==EINTR)          //��ȡ���ݵ�ʱ���ź��жϣ�������ȡ
    {
       continue;
    }
    else if(nread==-1&&((errno==EAGAIN)||(errno==EWOULDBLOCK)))  //ȫ���������Ѷ�ȡ���
    {
      std::string message;
      
      while(true)  //�ӽ��ջ������в�ֳ��ͻ��˵�������Ϣ
      {
        if(inputBuffer_.pickmessage(message)==false) break;
         
        lastatime_=Timestamp::now();                      //����Connecton��ʱ���
        //std::cout<<"last time="<<lastatime_.tostring()<<std::endl;
     
        onmessagecallback_(shared_from_this(),message); //�ص�TcpServer::onmessage()
      }
      break;
    }
    else if(nread==0)       //�ͻ��������ѶϿ�
    {
      closecallback();      //�ص�TcpServer::closecallback()
      break;
    }  
  }   
}

//��������,�������κ��߳��У����ǵ��ô˺�����������
void Connection::send(const char *data, size_t size)
{
  if(disconnect_==true){printf("client already diconnected.\n,send() back");return; }
  
  if(loop_->isinloopthread())    //�жϵ�ǰ�߳��Ƿ�Ϊ�¼�ѭ���߳�(IO�߳�)
  {
    sendinloop(data,size);
  }
  else
  {
    loop_->queueinloop(std::bind(&Connection::sendinloop,this,data,size));
  }
}

//�������ݣ������ǰ�߳���IO�̣߳�ֱ�ӵ��ô˺���������ǹ����̣߳����Ѵ˺�������IO�߳�
void Connection::sendinloop(const char *data, size_t size)   
{
  outputBuffer_.appendwithsep(data,size);   //����Ҫ���͵����ݱ��浽Connection�ķ��ͻ�������
  clientchannel_->enableWriting();    
}

void Connection::writecallback()
{
  int writen=::send(fd(),outputBuffer_.data(),outputBuffer_.size(),0);
  if(writen>0)outputBuffer_.erase(0,writen);  

  if(outputBuffer_.size()==0)
  {
    clientchannel_->disableWriting(); 
    sendcompletecallback_(shared_from_this());

  }
}

bool Connection::timeout(time_t now, int val)
{
  return now-lastatime_.toint()>val;
}






