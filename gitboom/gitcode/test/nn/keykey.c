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
    int buf[6];    
    fd_set readfds;    
    struct timeval tv;    
    // 打开键盘设备    
    fd = open( "/dev/input/event1", O_RDONLY );    
    // 判断是否打开成功    
    if(fd<0) {    
        printf("Failed to open \"/dev/input/event1\".\n");    
        exit(1);    
    } else {    
        printf("open \"/dev/input/event1\" successfuly.\n");    
    }    
    
    while(1) {    
        // 设置最长等待时间    
        tv.tv_sec = 5;    
        tv.tv_usec = 0;    
    
        FD_ZERO( &readfds );    
        FD_SET( 1, &readfds );    
    
        retval = select( fd+1, &readfds, NULL, NULL, &tv );    
        if(retval==0) {    
            printf( "Time out!\n" );    
        }    
        if(FD_ISSET(fd,&readfds)) {    
            // 读取键盘设备中的数据    
            if(read(fd, buf, 6) <= 0) {    
                continue;    
            }    
            // 打印出从键盘设备中读取到的数据    
            printf("Button type = %d, X = %d, Y = %d, Z = %d\n", buf[0], buf[1], buf[2], buf[3]);    
        }    
    }    
    close(fd);    
    return 0;    
}    