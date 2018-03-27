#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
 
//打印标识符的函数
void print_ids(const char *str)
{
    pid_t pid; //进程标识符
    pthread_t tid; //线程标识符
     
    pid=getpid();   //获得进程号
    tid=pthread_self(); //获得线程号
    printf("%s pid:%u tid:%u (0x%x)\n",
        str,(unsigned int)pid,(unsigned int)tid,(unsigned int)tid); //打印进程号和线程号
}
 
//线程函数
void* pthread_func(void *arg)
{
    print_ids("new thread:");  //打印新建线程号
    return ((void*)0);
}
 
//主函数
int main()
{
    int err;
    pthread_t ntid;    //线程号
     
    err=pthread_create(&ntid,NULL,pthread_func,NULL);   //创建一个线程
    if(err != 0)
    {
        printf("create thread failed:%s\n",strerror(err));
        exit(-1);
    }
    print_ids("main thread:"); //打印主线程号
    sleep(2);
    return 0;
}