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
#include <stdint.h>

#include "BSP_m24lr6e.h"

//#define BUF_SIZE 500


#define DEVICE_FILE		"/dev/i2c-2"
#define	SLAVE_ADDRESS	0x53


int Bsp_M24lr26e_Write(uint32_t addrStart,
	uint8_t *pBuff,
	uint16_t length)
{
	int32_t fd;
	fd = open(DEVICE_FILE, O_RDWR);
	if (!fd) {
		printf("Error on opening the device file\n");
		return -1;
	}

	ioctl(fd, I2C_TIMEOUT, 2);//设置超时时间
	ioctl(fd, I2C_RETRIES, 3);//设置重发次数

	printf("slave_addr is %2x \n", SLAVE_ADDRESS);
	printf("reg_addr is %2x \n", addrStart);


	struct i2c_rdwr_ioctl_data i2c_Data;

	i2c_Data.nmsgs = 1;
	i2c_Data.msgs = (struct i2c_msg *)malloc(
						i2c_Data.nmsgs * sizeof(struct i2c_msg));
	if (!i2c_Data.msgs) {
		printf("Memory alloc error\n");
		close(fd);
		return -1;
	}

	i2c_Data.msgs[0].addr = SLAVE_ADDRESS;
	i2c_Data.msgs[0].flags = 0;//写命令
	i2c_Data.msgs[0].buf = (unsigned char*)malloc(length + 2);

	printf("you have write : \n");

	for (uint32_t i = 0; i < length; i=i+4)
	{
		i2c_Data.msgs[0].buf[0] = (uint8_t)(((uint16_t)(addrStart+i)) >> 8);//信息值1 寄存器地址，即你要往哪写
		i2c_Data.msgs[0].buf[1] = (uint8_t)((uint16_t)(addrStart+i) & 0x00ff);//信息值1 寄存器地址，即你要往哪写
		
		uint32_t j;
		for (j = 0; (j < length - i) && (j < 4); j++)
		{
			i2c_Data.msgs[0].buf[j + 2] = pBuff[i+j];
		}
		i2c_Data.msgs[0].len = j+2;//length + 2u;//
		
		int ret = 0;
		ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_Data);//写进去ds2460
		if (ret < 0)
		{
			printf("ioctl write error\n");
			free(i2c_Data.msgs[0].buf);
			free(i2c_Data.msgs);
			close(fd);
			return -1;
		}
		else
		{
			for (j = 0; (j < length - i) && (j < 4); j++)
			{
				printf("%x ", i2c_Data.msgs[0].buf[j+2]);
			}
		}
		usleep(5000);
	}

	printf("\n into address %x address \n", addrStart);

	free(i2c_Data.msgs[0].buf);
	free(i2c_Data.msgs);
	close(fd);
	return 1;

}

int Bsp_M24lr26e_Read(uint32_t addrStart,
	uint8_t *pBuff,
	uint16_t length)
{
	int32_t fd;
	fd = open(DEVICE_FILE, O_RDWR);
	if (!fd) {
		printf("Error on opening the device file\n");
		return -1;
	}

	ioctl(fd, I2C_TIMEOUT, 20);//设置超时时间
	ioctl(fd, I2C_RETRIES, 3);//设置重发次数

	printf("slave_addr is %2x \n", SLAVE_ADDRESS);
	printf("reg_addr is %2x \n", addrStart);


	struct i2c_rdwr_ioctl_data i2c_Data;

	i2c_Data.nmsgs = 2;
	i2c_Data.msgs = (struct i2c_msg *)malloc(i2c_Data.nmsgs * sizeof(struct i2c_msg));
	if (!i2c_Data.msgs) {
		printf("Memory alloc error\n");
		close(fd);
		return -1;
	}

	i2c_Data.nmsgs = 2;//读时序要两次过程，要发两次I2C消息
	i2c_Data.msgs[0].len = 2;//信息长度为1，第一次只写要读的eeprom中存储单元的地址
	i2c_Data.msgs[0].addr = SLAVE_ADDRESS; //器件地址
	i2c_Data.msgs[0].flags = 0;//写命令，看读时序理解
	i2c_Data.msgs[0].buf = (unsigned char*)malloc(2);
	i2c_Data.msgs[0].buf[0] = (uint8_t)(((uint16_t)addrStart) >> 8);//信息值1 寄存器地址，即你要往哪写
	i2c_Data.msgs[0].buf[1] = (uint8_t)((uint16_t)addrStart & 0x00ff);//信息值1 寄存器地址，即你要往哪写

	i2c_Data.msgs[1].len = length;
	i2c_Data.msgs[1].addr = SLAVE_ADDRESS; //器件地址
	i2c_Data.msgs[1].flags = I2C_M_RD;//读命令*/
	i2c_Data.msgs[1].buf = (unsigned char*)malloc(length);
	memset(i2c_Data.msgs[1].buf, 0, length);//先清空要读的缓冲区

	int ret = 0;
	ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_Data);//
	if (ret < 0) {
		printf("ioctl read error\n");
		free(i2c_Data.msgs[0].buf);
		free(i2c_Data.msgs[1].buf);
		free(i2c_Data.msgs);
		close(fd);
		return -1;

	}
	else 
	{
		for (uint32_t i = 0; i < length; i++)
		{
			pBuff[i] = i2c_Data.msgs[1].buf[i];
		}

		printf("you have read : \n");
		for (uint32_t i = 0; i < length; i++)
		{
			printf("%x ", pBuff[i]);
		}
		printf("End \n");


		//i2c_Data.msgs[0].buf = (unsigned char*)malloc(2);
		//i2c_Data.msgs[1].buf = (unsigned char*)malloc(length);

		free(i2c_Data.msgs[0].buf);
		free(i2c_Data.msgs[1].buf);

		//i2c_Data.msgs[0].buf = (unsigned char*)malloc(2);
		//i2c_Data.msgs[1].buf = (unsigned char*)malloc(length);


		free(i2c_Data.msgs);
		close(fd);
		return 1;

	}

}

