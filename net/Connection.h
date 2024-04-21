#pragma once

#include<sys/syscall.h>
#include<functional>
#include<memory>
#include<atomic>

#include"Socket.h"
#include"EventLoop.h"
#include"InetAddress.h"
#include"Channel.h"
#include"EventLoop.h"
#include"Buffer.h"
#include"Timestamp.h"

/**********************************************
封装了服务端用于与客户端通讯的socket类，负责处理客户端传过来的信息

***********************************************/

class EventLoop;
class Channel;
class Connection;
using spConnection=std::shared_ptr<Connection>;

/*封装了服务端用于通信的channel*/
class Connection:public std::enable_shared_from_this<Connection>
{
private:
  EventLoop* loop_;    //Connection对应的事件循环，在构造函数中传入
  std::unique_ptr<Socket> clientsock_;   //与客户端通讯的Socket
  std::unique_ptr<Channel>clientchannel_;  //Connection对应的channel，在构造函数中创建
  Buffer inputBuffer_;     //接收缓冲区
  Buffer outputBuffer_;    //发送缓冲区
  std::atomic_bool disconnect_;   //客户端连接是否已断开，如果已断开，则设置为true
  
  std::function<void(spConnection)>closecallback_;  //关闭fd_的回调函数，将回调TcpServer：：closeconnection().
  std::function<void(spConnection)>errorcallback_;  //fd_发生了错误的回调函数，将回调TcpServer：：errorconnection().
  std::function<void(spConnection,std::string&)>onmessagecallback_; //处理报文的回调函数，将回调TcpServer::onmessage().
  std::function<void(spConnection)>sendcompletecallback_;  //处理报文的回调函数，将回调TcpServer：：onmessage().
  Timestamp lastatime_;                                    //时间戳,创建Connection对象时为当前时间，每接受到一个报文，把时间戳更新为当前时间
  

public:
  Connection(EventLoop* loop,std::unique_ptr<Socket> clientsock);
  ~Connection();
  
  int fd()const;          //返回客户端的fd_
  std::string ip()const;  //返回客户端的ip
  uint16_t port()const;   //返回客户端的port
  
  void onMessage();       //处理对端发送过来的消息
  void closecallback();   //TCP连接关闭(断开)的回调函数，供Channel回调
  void errorcallback();   //TCP连接错误的回调函数，供Channel回调
  void writecallback();   //处理写事件的回调函数，供Channel回调
  
  void setclosecallback(std::function<void(spConnection)>fn); //设置关闭fd_的回调函数
  void seterrorcallback(std::function<void(spConnection)>fn); //设置fd_发生了错误的回调函数
  void setonmessagecallback(std::function<void(spConnection,std::string&)>fn); //设置处理报文的回调函数
  void setsendcompletecallback(std::function<void(spConnection)>fn); //发送数据完成后的回调函数
  
  void send(const char *data, size_t size);  //发送数据,不管在任何线程中，都是调用此函数发送数据
  void sendinloop(const char *data, size_t size);   //如果是IO线程，直接调用此函数，如果是工作线程，将把此函数传给IO线程
  
  bool timeout(time_t now,int val);  //判断TCP连接是否超时,(空闲太久)
};
