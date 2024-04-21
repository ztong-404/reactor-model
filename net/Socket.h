#pragma once

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/tcp.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include"InetAddress.h"

//创建一个非阻塞的socket
int createnonblocking();

/*socket类
设置SO_REUSEADDR 选项
设置SO_REUSEPORT 选项
设置TCP_NODELAY 选项
设置SO_KEEPALIVE 选项
*/
class Socket    
{
  private:
    const int fd_; //Socket持有的fd
    std::string ip_;  //如果是listenfd，存放服务端监听的ip,如果是客户端连接的fd,存放对端的ip
    uint16_t port_;   //如果是listenfd，存放服务监听的port, 如果是客户端连接的fd，存放外部端口
  public:
    Socket(int fd); 
    ~Socket();
    
    int fd()const;  //返回fd_成员
    std::string ip()const;   //返回ip_成员
    uint16_t port()const;    //返回port_成员
    void setIpPort(const std::string &ip, uint16_t port); //设置ip_和port_成员
    
    void setReuseaddr(bool on); //设置SO_REUSEADDR 选项
    void setReuseport(bool on); //设置SO_REUSEPORT 选项
    void setTcpnodelay(bool on); //设置TCP_NODELAY 选项
    void setKeepalive(bool on); //设置SO_KEEPALIVE 选项

  void bind(const InetAddress& servaddr);
  void listen(int n=128);
  int accept(InetAddress& clientaddr);
};