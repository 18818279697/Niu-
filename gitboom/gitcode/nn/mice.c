#include <stdio.h>    
#include <stdlib.h>    
#include <linux/input.h>    
#include <fcntl.h>    
#include <sys/time.h>    
#include <sys/types.h>    
#include <sys/stat.h>    
#include <unistd.h>    
    
    
int main(int argc,char **argv)    
{    
    int fd, retval;    
    char buf[6];    
    fd_set readfds;    
    struct timeval tv;    
    // ������豸    
    fd = open( "/dev/input/mice", O_RDONLY );    
    // �ж��Ƿ�򿪳ɹ�    
    if(fd<0) {    
        printf("Failed to open \"/dev/input/mice\".\n");    
        exit(1);    
    } else {    
        printf("open \"/dev/input/mice\" successfuly.\n");    
    }    
    
    while(1) {    
        // ������ȴ�ʱ��    
        tv.tv_sec = 5;    
        tv.tv_usec = 0;    
    
        FD_ZERO( &readfds );    
        FD_SET( fd, &readfds );    
    
        retval = select( fd+1, &readfds, NULL, NULL, &tv );    
        if(retval==0) {    
            printf( "Time out!\n" );    
        }    
        if(FD_ISSET(fd,&readfds)) {    
            // ��ȡ����豸�е�����    
            if(read(fd, buf, 6) <= 0) {    
                continue;    
            }    
            // ��ӡ��������豸�ж�ȡ��������    
            printf("Button type = %d, X = %d, Y = %d, Z = %d\n", (buf[0] & 0x07), buf[1], buf[2],   buf[3]);    
        }    
    }    
    close(fd);    
    return 0;    
}    