#include"EventLoop.h"

int createtimerfd(int sec=30)
{
  int tfd=timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC|TFD_NONBLOCK);   //����timerfd
  struct itimerspec timeout;                                          //��ʱʱ������ݽṹ
  memset(&timeout,0,sizeof(struct itimerspec));
  timeout.it_value.tv_sec=sec;
  timeout.it_value.tv_nsec=0;
  timerfd_settime(tfd,0,&timeout,0);
  return tfd;
}

//���캯���д���epoll����
EventLoop::EventLoop(bool mainloop,int timetvl,int timeout):ep_(new Epoll),timetvl_(timetvl),timeout_(timeout),
          mainloop_(mainloop),stop_(false),
          wakeupfd_(eventfd(0,EFD_NONBLOCK)),wakechannel_(new Channel(this,wakeupfd_)),
          timerfd_(createtimerfd(timeout_)),timerchannel_(new Channel(this,timerfd_))
{
  wakechannel_->setreadcallback(std::bind(&EventLoop::handlewakeup,this));
  wakechannel_->enableReading();
  
  timerchannel_->setreadcallback(std::bind(&EventLoop::handletimer,this));
  timerchannel_->enableReading();
}
  
//��������������epoll����  
EventLoop::~EventLoop()
{
}

//�����¼�ѭ��
void EventLoop::run()
{
  threadid_=syscall(SYS_gettid);    //��ȡ�¼�ѭ�������̵߳�id
  
  while(stop_==false)
  {
      std::vector<Channel*>channels=ep_->loop(10*1000);  //�ȴ����ӵ�fd���¼�����

      if(channels.size()==0)
      {
        epolltimeoutcallback_(this);    
      }
      else
      {
        for(auto &ch:channels)
        {
          ch->handleEvent();
        }
      }
  }
}

//ֹͣ�¼�ѭ��
void EventLoop::stop()
{
  stop_=true;
  wakeup();   //�����¼�ѭ�������û�����д��룬�¼�ѭ�������´�������ʱ��epoll_wait()��ʱʱ�Ż�ͣ����
}

//��channel���/���µ�������ϣ�channel����fd��Ҳ����Ҫ���ӵ��¼�
void EventLoop::updatechannel(Channel *ch)
{
  ep_->updatechannel(ch);
}

//�Ӻ������ɾ��channel
void EventLoop::removechannel(Channel *ch)
{
  ep_->removechannel(ch);
}


//����epoll_wait()��ʱ�Ļص�����
void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)>fn)
{
  epolltimeoutcallback_=fn;
}

//�жϵ�ǰ�߳��Ƿ�Ϊ�¼�ѭ���̡߳�
bool EventLoop::isinloopthread()   
{
  return threadid_==syscall(SYS_gettid);
}

//��������ӵ�������
void EventLoop::queueinloop(std::function<void()>fn)
{
  {
    std::lock_guard<std::mutex>gd(mutex_);    //��������м���
    taskqueue_.push(fn);                      //�������
  }
   wakeup();  //�����¼�ѭ��
}

 //��eventfd�����¼�ѭ���߳�
void EventLoop::wakeup()
{
  uint64_t val=1;
  int temp=write(wakeupfd_,&val,sizeof(val));
}  

                        
//�¼�ѭ���̱߳�evenfd���Ѻ�ִ�еĺ��� 
void EventLoop::handlewakeup()
{

  uint64_t val;
  int temp=read(wakeupfd_,&val,sizeof(val));  //��evenfd�ж�ȡ�����ݣ��������ȡ��eventfd�Ķ��¼���һֱ����
  
  std::function<void()>fn;
  
  std::lock_guard<std::mutex>gd(mutex_); //��������м���
  
  //ִ�ж�����ȫ���ķ�������
  while(taskqueue_.size()>0)
  {
    fn=std::move(taskqueue_.front());    //����һ��Ԫ��
    taskqueue_.pop();         
    fn();                                //ִ������
  }
} 

void EventLoop::handletimer()  //������ʱִ�еĺ���
{
  //���¼�ʱ
  struct itimerspec timeout;                                          //��ʱʱ������ݽṹ
  memset(&timeout,0,sizeof(struct itimerspec));
  timeout.it_value.tv_sec=timetvl_;
  timeout.it_value.tv_nsec=0;
  timerfd_settime(timerfd_,0,&timeout,0);
  
  if(!mainloop_)
    {
      time_t now=time(0);   //��ȡ��ǰʱ��
      
      //ѭ��ʱɾ����
      for(auto it=conns_.begin();it!=conns_.end();)
      {
       // std::cout<<it->first<<" ";
        if(it->second->timeout(now,timeout_))
        {
          timercallback_(it->first);
          std::lock_guard<std::mutex>lock(mmutex_);
          std::cout<<"EventLoop::HandleTimer() thread is "<<syscall(SYS_gettid)<<" fd\n";
          conns_.erase(it++);
        }
        else
          it++;
       }   
    }

}

//��Connection���󱣴���conns_��
void EventLoop::newconnection(spConnection conn)
{
  std::lock_guard<std::mutex>gd(mmutex_);
  conns_[conn->fd()]=conn;
}

//��������ΪTcpServer::removeconn()
void EventLoop::settimercallback(std::function<void(int)>fn)
{
  timercallback_=fn;
}

