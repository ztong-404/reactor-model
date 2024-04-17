#pragma once

//#include<cstdint>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string>

/*
socket的地址协议类
获取地址ip和端口号，
负责返还地址IP和端口号，还有addr_成员的地址
设置addr_成员的值
*/
class InetAddress
{
  private:
    sockaddr_in addr_; //表示地址协议的结构体
  public:
    InetAddress();
    InetAddress(const std::string &ip, uint16_t port);
    InetAddress(const sockaddr_in addr);//客户端连上来的fd
    ~InetAddress();
    
    const char *ip()const; //返回字符串表示的地址
    uint16_t port()const;  //返回整数表示的端口
    const sockaddr *addr()const;  //返回addr_成员的地址，InetAddress类 转化成sockadddr类
    void setaddr(sockaddr_in clientaddr);  //设置addr_成员的值
};