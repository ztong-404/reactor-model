#include"BankServer.h"

BankServer::BankServer(const std::string &ip,const uint16_t port, int subthreadnum, int workthreadnum)
          :tcpserver_(ip,port,subthreadnum),threadpool_(workthreadnum,"WORKS")
{
  tcpserver_.setnewconnectioncb(std::bind(&BankServer::HandleNewConnection,this,std::placeholders::_1));
  tcpserver_.setcloseconnectioncb(std::bind(&BankServer::HandleClose,this,std::placeholders::_1));
  tcpserver_.seterrorconnectioncb(std::bind(&BankServer::HandleError,this,std::placeholders::_1));
  tcpserver_.setonmessagecb(std::bind(&BankServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2));
  tcpserver_.setsendcompletecb(std::bind(&BankServer::HandleSendComplete,this,std::placeholders::_1));
  tcpserver_.setremoveconnectioncb(std::bind(&BankServer::HandleRemove,this,std::placeholders::_1));
}

BankServer::~BankServer()
{}

//��������
void BankServer::Start()
{
  tcpserver_.start();
}

//ֹͣ����
void BankServer::Stop()
{
  //ֹͣ�����߳�
  threadpool_.stop();
  printf("thread working stop.\n");
  //ֹͣIO�̣߳��¼�ѭ����
  tcpserver_.stop();
}
  
 //�����¿ͻ�������������TcpServer���лص��˺���  
void BankServer::HandleNewConnection(spConnection conn)
{ 
   spUserInfo userinfo(conn->fd(),conn->ip());  
   {
     std::lock_guard<std::mutex>gd(mutex_);
     usermap_[conn->fd()]=userinfo;              //���û���ӵ�״̬����
   } 
    
} 

//�رտͻ��˵����ӣ���TcpServer���лص��˺���
void BankServer::HandleClose(spConnection conn)
{
   printf("%s The connection has been dropped (ip=%s).\n",Timestamp::now().tostring().c_str(),conn->ip().c_str());
   {
     std::lock_guard<std::mutex>gd(mutex_);
     usermap_.erase(conn->fd());                //��״̬����ɾ���û���Ϣ
   }
}

//�ͻ��˵����Ӵ�����TcpServer���лص��˺���
void BankServer::HandleError(spConnection conn)
{
   HandleClose(conn);
} 

//����ͻ��˵����ʱ��ģ���TcpServer���лص��˺���
void BankServer::HandleMessage(spConnection conn,std::string& message)
{
   if(threadpool_.size()==0)
   {
     //���û�й����̣߳���ʾ��IO�߳��м���
     OnMessage(conn,message);
   } 
   else
   {
     //��ҵ����ӵ��̳߳ص����������
     threadpool_.addtask(std::bind(&BankServer::OnMessage,this,conn,message));
   }
}  

//����ͻ��˵������ģ�������Ӹ��̳߳�
void BankServer::OnMessage(spConnection conn, std::string &message) 
{
  spUserInfo userinfo=usermap_[conn->fd()];  //��״̬���л�ȡ�ͻ��˵���Ϣ
  
  //�����ͻ��˵�������
  std::string bizcode;         //ҵ�����
  std::string replaymessage;   //��Ӧ����
  getxmlbuffer(message,"bizcode",bizcode);  //���������н�����ҵ�����
  
  if(bizcode=="00101")  //��¼ҵ��
  {
    std::string username,password;
    getxmlbuffer(message,"username",username);
    getxmlbuffer(meesage,"password",password);
    if(username=="Tong")&&(password=="123456")   //��������ݿ��ѯ�����û���������
    {
      replaymessage="<bizcode>00102</bizcode><retcode>0</retcode><message>ok</message>";
      userinfo->setLogin(true);                  //�����û��ĵ�¼״̬Ϊtrue
    }
    else
    {
      replaymessage="<bizcode>00102</bizcode><retcode>-1</retcode><message>y=username or password are wrong.</message>";
    }
  }
  else if(bizcode=="00201")    //��ѯ���ҵ��
  {
    if(userinfo->Login()==true)
    {
      replaymessage="<bizcode>00202</bizcode><retcode>0</retcode><message>5088.80</message>";  //��������ݿ�鵽���û����
    }
    else
    {
      replaymessage="<bizcode>00202</bizcode><retcode>-1</retcode><message>user Login failed.</message>"
    }
  }
  else if(bizcode=="00901")   //ע��ҵ��
  {
    if(userinfo->Login()==true)
    {
      replaymessage="<bizcode>00902</bizcode><retcode>0</retcode><message>ok</message>"
      userinfo->setLogin(false);
    }
    else
    {
      replaymessage="<bizcode>00902</bizcode><retcode>-1</retcode><message>user Login failed.</message>"
    }
  }
  else if(bizcode=="00001")
  {
    if(userinfo->Login()==true)
    {
      replaymessage="<bizcode>00002</bizcode><retcode>0</retcode><message>ok</message>";
    }
    else
    {
      replaymessage="<bizcode>00002</bizcode><retcode>-1</retcode><message>user Login failed.</message>"
    }
  }
  conn->send(replaymessage.data(),replaymessage.size()); //�����ݷ��ͳ�ȥ
}


 //���ݷ�����ɺ���TcpServer���лص��˺���
void BankServer::HandleSendComplete(spConnection conn)
{
  //std::cout<<"Message send complete."<<std::endl;
}    

void BankServer::HandleRemove(int fd)
{
  printf("fd(%d)Timed out.",fd);
  
  std::lock_guard<std::mutex>gd(mutex_);
  usermap_.erase(fd);
}


