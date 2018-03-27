#include <stdio.h>   
#include <linux/types.h>   
#include <fcntl.h>   
#include <unistd.h>   
#include <stdlib.h>   
#include <sys/types.h>   
#include <sys/ioctl.h>   
#include <errno.h>   
#include <assert.h>   
#include <string.h>   
#include <linux/i2c.h>   
#include <linux/i2c-dev.h>   
   
int main()  
{  
         int fd, ret;  
         unsigned char rdwr_addr = 0xf8;   /* ����Ƭ�ڱ�ַ0x00~oxff,e2prom ��д��ַ */  
         unsigned char device_addr = 0x53; /* e2prom �豸��ַ */  
         unsigned char data = 0x18;  /* ��e2promд�����ݣ����ֽڷ��� */  
         struct i2c_rdwr_ioctl_data e2prom_data;  
   
         fd = open("/dev/i2c-2", O_RDWR);  
         if(fd < 0) {  
                   perror("openerror");  
                   exit(1);  
         }  
   
         e2prom_data.msgs= (struct i2c_msg *)malloc(e2prom_data.nmsgs *sizeof(struct i2c_msg));  
         if(e2prom_data.msgs == NULL)
	     {  
                   perror("mallocerror");  
                   exit(1);  
         }  
   
         ioctl(fd,I2C_TIMEOUT, 1); /* ���ó�ʱ */  
         ioctl(fd,I2C_RETRIES, 2); /* �������Դ��� */  
   
          
         /*��e2prom��rdwr_addr��ַд������data*/  
         e2prom_data.nmsgs= 1;  
         e2prom_data.msgs[0].len= 2;  
         e2prom_data.msgs[0].addr= device_addr;  
         e2prom_data.msgs[0].flags= 0;     /* write */  
   
          
         e2prom_data.msgs[0].buf= (unsigned char *)malloc(2);  
         e2prom_data.msgs[0].buf[0]= rdwr_addr;    /* write address */  
         e2prom_data.msgs[0].buf[1]= data;      /* write data */  
   
         ret= ioctl(fd, I2C_RDWR, (unsigned long)&e2prom_data);  
         if(ret < 0) {  
                   perror("writedata error");  
                   exit(1);  
         }  
         printf("writedata: %d to address: %#x\n", data, rdwr_addr);  
         data= 0;  /* be zero*/  
   
   
         /*��e2prom��rdwr_addr��ַ��ȡ���ݴ���buf*/  
         e2prom_data.nmsgs= 2;  
         e2prom_data.msgs[0].len= 1;  
         e2prom_data.msgs[0].addr= device_addr;  
         e2prom_data.msgs[0].flags= 0;     /* write */   
         e2prom_data.msgs[0].buf= (unsigned char *)malloc(1);
         e2prom_data.msgs[0].buf[0]= rdwr_addr;  
   
         e2prom_data.msgs[1].len= 1;  
         e2prom_data.msgs[1].addr= device_addr;  
         e2prom_data.msgs[1].flags= I2C_M_RD;     /* read */  
         e2prom_data.msgs[1].buf= (unsigned char *)malloc(1);
         e2prom_data.msgs[1].buf[0]= data;  
   
         ret= ioctl(fd, I2C_RDWR, (unsigned long)&e2prom_data);  
         if(ret < 0) {  
                   perror("readerror");  
                   exit(1);  
         }  
         printf("read  data: %x from address: %#x\n", e2prom_data.msgs[1].buf[0],rdwr_addr);  
          
         free(e2prom_data.msgs);  
         close(fd);  
   
         return 0;  
}  