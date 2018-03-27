#include <stdio.h>
#include <linux/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#define BUF_SIZE 500
struct i2c_rdwr_ioctl_data ds2460_date;
unsigned int fd,i,j;
unsigned int slave_address, reg_address,length,value;
int ret;
char n=0 ,rd_wr;
char buf[BUF_SIZE];
int main(int argc, char **argv)
{
	if (argc < 5)
		{
		printf("Usage:\n%s /dev/i2c/x start_addr reg_addr value\n",argv[0]);
		return 0;                                                                                          
	    }
	fd = open(argv[1], O_RDWR);
	if (!fd)
		{
		printf("Error on opening the device file\n");
		return 0;
	    }
    slave_address= strtoul(argv[2], NULL, 0);
    reg_address= strtoul(argv[3], NULL, 0);
    length= strtoul(argv[4], NULL, 0);
    value= strtoul(argv[5], NULL, 0);
	//rd_wr=slave_address %2;
	slave_address=slave_address/2;
    printf("slave_addr is %2x \n",slave_address);
    printf("reg_addr is %2x \n",reg_address);
	ds2460_date .nmsgs = 2;//因为都时序要两次，所以设为2
	ds2460_date .msgs = (struct i2c_msg *)malloc(ds2460_date.nmsgs * sizeof(struct i2c_msg));
	if (!ds2460_date.msgs)
		{
	printf("Memory alloc error\n");
	close(fd);
	return 0;
	    }
	ioctl(fd, I2C_TIMEOUT, 2);//设置超时时间
	ioctl(fd, I2C_RETRIES, 1);//设置重发次数
//	if(rd_wr==0){
	    ds2460_date.nmsgs = 1;
		ds2460_date.msgs[0].len = length+2;//
		ds2460_date.msgs[0].addr = slave_address;
		ds2460_date.msgs[0].flags = 0;//写命令
		ds2460_date.msgs[0].buf = (unsigned char*)malloc(length+2);
		ds2460_date.msgs[0].buf[0] = reg_address >>8;//信息值1 寄存器地址，即你要往哪写
		ds2460_date.msgs[0].buf[1] = reg_address & 0xff;//信息值1 寄存器地址，即你要往哪写
		/*for(i=1,j=5;j<argc;j++,i++){
			sscanf(argv[j], "%x", &value);
			ds2460_date.msgs[0].buf[i] = value;//信息值2,即你要写什么
		}*/
		 memset(ds2460_date.msgs[0].buf+2,value,length);
		ret = ioctl (fd, I2C_RDWR, (unsigned long)&ds2460_date);//写进去ds2460
		if (ret < 0)
			{
			printf ("ioctl write error\n");
		    }
		else{
			n=snprintf(buf,BUF_SIZE,"you have write :\n ");
			for(i=2;i<length+2;i++)
			{
				n +=snprintf(buf+n,BUF_SIZE-n," %02x\n ",ds2460_date.msgs[0].buf[i]);
			}
			n+=snprintf(buf+n,BUF_SIZE-n,"in to address %02x address\n",reg_address);

			printf("%s\n",buf);
		    }
        free(ds2460_date.msgs[0].buf);
		free(ds2460_date.msgs);
	//}
	//else if(rd_wr==1){
	ds2460_date.nmsgs = 2;//读时序要两次过程，要发两次I2C消息
	ds2460_date.msgs[0].len = 2;//信息长度为1，第一次只写要读的eeprom中存储单元的地址
	ds2460_date.msgs[0].addr = slave_address; //器件地址
	ds2460_date.msgs[0].flags = 0;//写命令，看读时序理解
	ds2460_date.msgs[0].buf = (unsigned char*)malloc(2);
	ds2460_date.msgs[0].buf[0] = reg_address >> 8;//要写入数据的单元地址
	ds2460_date.msgs[0].buf[1] = reg_address & 0xff;//要写入数据的单元地址
	
	ds2460_date.msgs[1].len = length;
	ds2460_date.msgs[1].addr = slave_address; //器件地址
	ds2460_date.msgs[1].flags = I2C_M_RD;//读命令*/
	ds2460_date.msgs[1].buf = (unsigned char*)malloc(length);
	memset(ds2460_date.msgs[1].buf,0,length);//先清空要读的缓冲区
	ret = ioctl (fd, I2C_RDWR, (unsigned long)&ds2460_date);//好了，读吧
	if (ret < 0)
		{
		printf ("ioctl read error\n");
	    }
    else{
		n=snprintf(buf,BUF_SIZE,"you have read :\n");
		for(i=0;i<length;i++)
		{
			n +=snprintf(buf+n,BUF_SIZE-n,"%02x\n",ds2460_date.msgs[1].buf[i]);
		}
		n+=snprintf(buf+n,BUF_SIZE-n,"from ds2460 address %02x\n",reg_address);
		printf("%s \n", buf);
	    }
	 free(ds2460_date.msgs[0].buf);
	 free(ds2460_date.msgs[1].buf);

	//}
	//else{
	printf("please config read or write to i2c device !");
	//}
	free(ds2460_date.msgs);
	close(fd);
	return 0; 
}


