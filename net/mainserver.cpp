#include"EchoServer.h"

#include<signal.h>

EchoServer *echoserver;

void Stop(int sig)  //信号2和15的处理函数，功能是停止服务程序
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
  
  signal(SIGTERM,Stop);  //信号15，系统kill或killall命令默认发送的信号
  signal(SIGINT,Stop);   //信号2，按ctrl+C发出信号
  
  echoserver =new EchoServer(argv[1],atoi(argv[2]),30,0); //io线程设置为30，工作线程为0
  echoserver->Start();

  return 0;
}