#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
 
//��ӡ��ʶ���ĺ���
void print_ids(const char *str)
{
    pid_t pid; //���̱�ʶ��
    pthread_t tid; //�̱߳�ʶ��
     
    pid=getpid();   //��ý��̺�
    tid=pthread_self(); //����̺߳�
    printf("%s pid:%u tid:%u (0x%x)\n",
        str,(unsigned int)pid,(unsigned int)tid,(unsigned int)tid); //��ӡ���̺ź��̺߳�
}
 
//�̺߳���
void* pthread_func(void *arg)
{
    print_ids("new thread:");  //��ӡ�½��̺߳�
    return ((void*)0);
}
 
//������
int main()
{
    int err;
    pthread_t ntid;    //�̺߳�
     
    err=pthread_create(&ntid,NULL,pthread_func,NULL);   //����һ���߳�
    if(err != 0)
    {
        printf("create thread failed:%s\n",strerror(err));
        exit(-1);
    }
    print_ids("main thread:"); //��ӡ���̺߳�
    sleep(2);
    return 0;
}