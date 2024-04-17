// ����ͨѶ�Ŀͻ��˳���
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage:./client ip port\n"); 
        printf("example:./client 192.168.48.128 5085\n\n"); 
        return -1;
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char buf[1024];
 
    if ((sockfd=socket(AF_INET,SOCK_STREAM,0))<0) { printf("socket() failed.\n"); return -1; }
    
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)
    {
        printf("connect(%s:%s) failed.\n",argv[1],argv[2]); close(sockfd);  return -1;
    }

    printf("connect ok.\n");
    printf("start time: %ld\n",time(0));

    for (int ii=0;ii<100000;ii++)
    {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"number: %d ",ii);

        char tmpbuf[1024];                 // ��ʱ��buffer������ͷ��+�������ݡ�
        memset(tmpbuf,0,sizeof(tmpbuf));
        int len=strlen(buf);                 // ���㱨�ĵĴ�С��
        memcpy(tmpbuf,&len,4);       // ƴ�ӱ���ͷ����
        memcpy(tmpbuf+4,buf,len);  // ƴ�ӱ������ݡ�

        send(sockfd,tmpbuf,len+4,0);  // �������ķ��͸�����ˡ�
        
        recv(sockfd,&len,4,0);            // �ȶ�ȡ4�ֽڵı���ͷ����

        memset(buf,0,sizeof(buf));
        recv(sockfd,buf,len,0);           // ��ȡ�������ݡ�

        // printf("recv:%s\n",buf);
    }
    printf("end time: %ld\n",time(0));


} 