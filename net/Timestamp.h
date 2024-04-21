#pragma once
#include<iostream>
#include<string>

class Timestamp
{
private:
  time_t secsinceepoch_;  //������ʾʱ��(��1970����������ȥ������)  
public:
  Timestamp();                      //�õ�ǰʱ���ʼ������
  Timestamp(int64_t secsinceepoch);  //��һ��������ʾ��ʱ���ʼ������
  
  static Timestamp now();            //���ص�ǰʱ���Timestamp����
  
  time_t toint()const;             //����������ʾ��ʱ��
  std::string tostring()const;     //�����ַ�����ʾ��ʱ�䣬��ʽ:yyyy-mm-dd hh24:mi::ss
};