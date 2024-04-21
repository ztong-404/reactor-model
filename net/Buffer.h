#pragma once
#include<string>
#include<cstring>
#include<iostream>

class Buffer
{
private:
  std::string buf_;  //���ڴ������
  const uint16_t sep_; //���ĵķָ�����0-�޷ָ�����1-���ֽڵİ�ͷ;2-"\r\n\r\n"�ָ�����httpЭ�飩
  
public:
  Buffer(uint16_t sep=1);
  ~Buffer();
  
  void append(const char* data, size_t size);  //��������ӵ�buf_��
  void appendwithsep(const char *data,size_t size); //������׷�ӵ�buf_��
  void erase(size_t pos, size_t nn);         //��buf_��pos��ʼ��ɾ��nn���ֽڣ�pos��0��ʼ
  size_t size();          //����buf�Ĵ�С
  const char *data();     //����buf���׵�ַ
  void clear();           //���buf_
  bool pickmessage(std::string &ss);     //��buf_�в�ֳ�һ�����ģ������ss�У����buf_��û�б��ģ�����false
};