#pragma once

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/tcp.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include"InetAddress.h"

//����һ����������socket
int createnonblocking();

/*socket��
���ð󶨺ͼ����ͻ��˶˿ڣ�
����ip_��port_��Ա�� fd_��Ա��Socket���е�fd��
aceept�����᷵���µ�fd��������Զ�ͨ��
���ֲ�:
����SO_REUSEADDR ѡ��
����SO_REUSEPORT ѡ��
����TCP_NODELAY ѡ��
����SO_KEEPALIVE ѡ��
*/
class Socket    
{
  private:
    const int fd_; //Socket���е�fd
    std::string ip_;  //�����listenfd����ŷ���˼�����ip,����ǿͻ������ӵ�fd,��ŶԶ˵�ip
    uint16_t port_;   //�����listenfd����ŷ��������port, ����ǿͻ������ӵ�fd������ⲿ�˿�
  public:
    Socket(int fd); 
    ~Socket();
    
    int fd()const;  //����fd_��Ա
    std::string ip()const;   //����ip_��Ա
    uint16_t port()const;    //����port_��Ա
    void setIpPort(const std::string &ip, uint16_t port); //����ip_��port_��Ա
    
    void setReuseaddr(bool on); //����SO_REUSEADDR ѡ��
    void setReuseport(bool on); //����SO_REUSEPORT ѡ��
    void setTcpnodelay(bool on); //����TCP_NODELAY ѡ��
    void setKeepalive(bool on); //����SO_KEEPALIVE ѡ��

  void bind(const InetAddress& servaddr);
  void listen(int n=128);
  int accept(InetAddress& clientaddr);
};