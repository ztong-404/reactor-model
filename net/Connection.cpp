#include"Connection.h"

Connection::Connection(EventLoop* loop,std::unique_ptr<Socket> clientsock)
            :loop_(loop),clientsock_(std::move(clientsock)),disconnect_(false),clientchannel_(new Channel(loop_,clientsock_->fd()))
{
  //为新客户端准备读事件，并添加到epoll中
  clientchannel_->setreadcallback(std::bind(&Connection::onMessage,this));
  clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
  clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
  clientchannel_->setwritecallback(std::bind(&Connection::writecallback,this));
  clientchannel_->useET();  //客户端连接上来的fd采用边缘触发
  clientchannel_->enableReading();  //让epoll_wait()监视clientchannel的读事件
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
  clientchannel_->remove();    //从事件循环中删除Channel
  closecallback_(shared_from_this());
}

void Connection::errorcallback()
{
  disconnect_=true;
  clientchannel_->remove();    //从事件循环中删除Channel
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


//处理对端发送过来的消息
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
    else if(nread==-1&&errno==EINTR)          //读取数据的时候被信号中断，继续读取
    {
       continue;
    }
    else if(nread==-1&&((errno==EAGAIN)||(errno==EWOULDBLOCK)))  //全部的数据已读取完毕
    {
      std::string message;
      
      while(true)  //从接收缓存区中拆分出客户端的请求消息
      {
        if(inputBuffer_.pickmessage(message)==false) break;
         
        lastatime_=Timestamp::now();                      //更新Connecton的时间戳
        //std::cout<<"last time="<<lastatime_.tostring()<<std::endl;
     
        onmessagecallback_(shared_from_this(),message); //回调TcpServer::onmessage()
      }
      break;
    }
    else if(nread==0)       //客户端连接已断开
    {
      closecallback();      //回调TcpServer::closecallback()
      break;
    }  
  }   
}

//发送数据,不管在任何线程中，都是调用此函数发送数据
void Connection::send(const char *data, size_t size)
{
  if(disconnect_==true){printf("client already diconnected.\n,send() back");return; }
  
  if(loop_->isinloopthread())    //判断当前线程是否为事件循环线程(IO线程)
  {
    sendinloop(data,size);
  }
  else
  {
    loop_->queueinloop(std::bind(&Connection::sendinloop,this,data,size));
  }
}

//发送数据，如果当前线程是IO线程，直接调用此函数，如果是工作线程，将把此函数传给IO线程
void Connection::sendinloop(const char *data, size_t size)   
{
  outputBuffer_.appendwithsep(data,size);   //把需要发送的数据保存到Connection的发送缓冲区中
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






