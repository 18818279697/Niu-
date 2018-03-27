#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
struct input_event buff; 
int fd; 
int read_nu;
int main(int argc, char *argv[])
{
    fd = open("/dev/input/event1", O_RDONLY);
    if (fd < 0)
    { 
        perror("can not open device usbkeyboard!"); 
        exit(1); 
    } 
    int i = 0;
    printf("--fd:%d--\n",fd);
    while(1)
    {
        while(read(fd,&buff,sizeof(struct input_event))==0)
        {
             ;
        }
        //if(buff.code > 40)
        printf("type:%d code:%d value:%d\n",buff.type,buff.code,buff.value); 
        printf("**************************\n");

    } 
    
    close(fd); 
    return 1;
}