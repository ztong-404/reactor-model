#include"Buffer.h"

Buffer::Buffer(uint16_t sep):sep_(sep)
{}

Buffer::~Buffer()
{}

//��������ӵ�buf_��
void Buffer::append(const char* data, size_t size)
{
  buf_.append(data,size);
}

//������׷�ӵ�buf_��
void Buffer::appendwithsep(const char *data,size_t size)
{
  if(sep_==0)
  {
    buf_.append(data,size);   //����������
  }
  else if(sep_==1)  //���ֽڱ�ͷ
  {
  buf_.append((char*)&size,4); //�����ĳ���
  buf_.append(data,size);      //����������
  }
  else if(sep_==2)  //"\r\n\r\n"�ָ���
  {
    
  }
}

//����buf�Ĵ�С
size_t Buffer::size()
{
  return buf_.size();
}
         
//����buf���׵�ַ
const char *Buffer::data()
{
  return buf_.data();
}

//���buf
void Buffer::clear()
{
  return buf_.clear();
}

//��buf_��pos��ʼ��ɾ��nn���ֽڣ�pos��0��ʼ
void Buffer::erase(size_t pos, size_t nn)
{
  buf_.erase(pos,nn);
}

//��buf_�в�ֳ�һ�����ģ������ss�У����buf_��û�б��ģ�����false
bool Buffer::pickmessage(std::string &ss)
{
  if(buf_.size()==0)return false;
  
  if(sep_==0)   //û�зָ���
  {
    ss=buf_;
    buf_.clear();
  }
  else if(sep_==1)    //���ֽڵı�ͷ
  {
    int len;
    memcpy(&len,buf_.data(),4);   //��buf_�л�ȡ����ͷ��
    if(buf_.size()<len+4)return false;  //���buf_�е�������С�ڱ���ͷ����˵��buf_�еı������ݲ�����
        
    ss=buf_.substr(4,len);        //��buf_�л�ȡһ������
    buf_.erase(0,len+4);          //��buf_��ɾ���ղ��ѻ�ȡ�ı���
  }
  
  return true;
}