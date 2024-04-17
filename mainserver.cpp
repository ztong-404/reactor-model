#include"EchoServer.h"

#include<signal.h>

EchoServer *echoserver;

void Stop(int sig)  //�ź�2��15�Ĵ�������������ֹͣ�������
{
  printf("sig=%d\n",sig);
  echoserver->Stop();
  printf("mainserver already stop.\n");
  delete echoserver;
  exit(0);
}

int main(int argc,char* argv[])
{
  if(argc!=3)
  {
    printf("usage:./mainserver ip port\n");
    printf("example:./mainserver 192.168.48.128 5085\n");
    return -1;
  }
  
  signal(SIGTERM,Stop);  //�ź�15��ϵͳkill��killall����Ĭ�Ϸ��͵��ź�
  signal(SIGINT,Stop);   //�ź�2����ctrl+C�����ź�
  
  echoserver =new EchoServer(argv[1],atoi(argv[2]),3,2);
  echoserver->Start();

  return 0;
}