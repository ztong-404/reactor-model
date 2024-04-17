#pragma once

//#include<cstdint>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string>

/*
socket�ĵ�ַЭ����
��ȡ��ַip�Ͷ˿ںţ�
���𷵻���ַIP�Ͷ˿ںţ�����addr_��Ա�ĵ�ַ
����addr_��Ա��ֵ
*/
class InetAddress
{
  private:
    sockaddr_in addr_; //��ʾ��ַЭ��Ľṹ��
  public:
    InetAddress();
    InetAddress(const std::string &ip, uint16_t port);
    InetAddress(const sockaddr_in addr);//�ͻ�����������fd
    ~InetAddress();
    
    const char *ip()const; //�����ַ�����ʾ�ĵ�ַ
    uint16_t port()const;  //����������ʾ�Ķ˿�
    const sockaddr *addr()const;  //����addr_��Ա�ĵ�ַ��InetAddress�� ת����sockadddr��
    void setaddr(sockaddr_in clientaddr);  //����addr_��Ա��ֵ
};