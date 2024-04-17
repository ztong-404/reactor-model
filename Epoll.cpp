#include"Epoll.h"

//Epoll��
Epoll::Epoll()
{
  if((epollfd_=epoll_create(1))==-1) //����epoll���(�����)
    {
      printf("epoll_create() failed(%d).\n",errno);exit(-1);
    }
}

Epoll::~Epoll()//�ر�epollfd
{
  close(epollfd_);
}

//��channel���/���µ�������ϣ�channel����fd��Ҳ����Ҫ���ӵ��¼�
void Epoll::updatechannel(Channel *ch)    
{
  epoll_event ev;   //�����¼��Ľṹ��
  ev.data.ptr=ch;
  ev.events=ch->events();
  
  if(ch->inpoll())    //���channel�Ѿ��ں������
  {
    if(epoll_ctl(epollfd_,EPOLL_CTL_MOD,ch->fd(),&ev)==-1)
    {
      perror("epoll_ctl()failed.\n");exit(-1);
    }
  }
  else         //���channel��������
  {           
     if(epoll_ctl(epollfd_,EPOLL_CTL_ADD,ch->fd(),&ev)==-1)
     {
      perror("epoll_ctl()failed.\n");exit(-1);
     }
     ch->setinepoll(true);  //��channel��inepoll_��Ա����Ϊtrue
  }
}

//�Ӻ������ɾ��channel
void Epoll::removechannel(Channel *ch)
{
  if(ch->inpoll())    //���channel�Ѿ���������
  {
    printf("removechannel()\n");
    if(epoll_ctl(epollfd_,EPOLL_CTL_DEL,ch->fd(),0)==-1)
    {
      perror("epoll_ctl() failed.\n"); exit(-1);
    }
  }
}


std::vector<Channel*>Epoll::loop(int timeout)    //����epoll_wait()���ȴ��¼��ķ������ѷ������¼���vector��������
{
 std::vector<Channel*>channels; //���epoll_wait()���ص��¼�
  
  bzero(events_,sizeof(events_));
  int infds=epoll_wait(epollfd_,events_,MaxEvents,timeout); //�ȴ����ӵ�fd���¼��������ú���������Ҫ������¼���Ŀ
  
  //����ʧ��
  if(infds<0)
  {
    perror("epoll_wait()failed");exit(-1);
  }
  
  //��ʱ
  if(infds==0)
  {
    //���epoll_wait()��ʱ����ʾϵͳ�ܿ��У����ص�channels��Ϊ��
    printf("epoll_wait() timeout.\n");
    return channels;
  }
  
  //���infds>0, ��ʾ���¼�������fd������
  for(int ii=0;ii<infds;ii++)
  {
    //evs.push_back(events_[ii]);  //����epoll���ص�����events
    Channel *ch=(Channel*)events_[ii].data.ptr;
    ch->setrevents(events_[ii].events);
    channels.push_back(ch);
  }
  return channels;  

}
