//============================================================================
// Name        : 0.cpp
// Author      : Carius Fang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include "../Modbus/modbusTcp/MbTcpServer.h"
#include "../Modbus/modbusSlaveStack/MbInterface.h"
#include "../app/Database.h"
#include "../Modbus/ModbusThread/MbProcessThread.h"
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>

//#include "../random-test-client.h"
#include "../Modbus/modbusTcp/MbTcpClient.h"
#include "../CanBackplane/canBp_Interface.h"
#include "../CanBackplane/canBp_sys_arch.h"
#include "../fileSysApp/fatFS_app.h"
void ExitProgram(void);


pthread_t threadID_ModbusTcp;
pthread_t threadID_ModbusProcess;
pthread_t threadID_ModbusMasterTest;
pthread_t threadID_SPITest;
pthread_t threadID_GPIOTest;



static struct sigaction struAct; //信号设置
static struct sigaction struOldAct; //原信号设置

void DeleteMsgQueue();
void CaptureQuitSignal(int nSignal);
//void display();
//void spiTest();
//void gpioTest();


#include "../BSP/BSP_m24lr6e.h"
int main() {
	int errLog;
	printf("Hello world!\n");

	/*数据初始化*/
	InitData();
	usRegHoldingBuf=(uint16_t *)pDataAO;
	usRegInputBuf=(uint16_t *)pDataAI;
	ucRegCoilsBuf=pDataDO;
	ucRegDiscreteBuf=pDataDI;

	printf("Data Initiate Complete!\n");


	/*设置信号处理*/
	struAct.sa_handler=CaptureQuitSignal;
	struAct.sa_flags=0;
	sigfillset(&struAct.sa_mask);
	if(-1 == sigaction(SIGINT,&struAct,&struOldAct))
	{
		printf("[ERRORS] sigaction is failed!\n");
		exit(0);
	}

	printf("Create ModbusMessgeQueue ...\n");
	key_t msgQKey=ftok(".",				//当前路径
						ID_PROJ_MSGQ);//项目ID
	if(-1 == msgQKey)
	{
		ExitProgram();
		perror("msgQ ftok"),exit(-1);
	}
	
	msgQId=msgget(msgQKey,	//消息key
				IPC_CREAT/*|IPC_EXCL*/|0644);//创建，且已创建即失败，给权限
	if(-1 == msgQId)
	{
		ExitProgram();
		perror("msgget"),exit(-1);
	}
	printf("msgQId = %d\n",msgQId);



	/*--canBackplane初始化--*/
	pRamDataDI = pDataDI;
	pRamDataDO = pDataDO;
	pRamDataAI = pDataAI;
	pRamDataAO = pDataAO;
	pRamShMemIOStatus = pShareMemoIOStatus;
	pRamIONodeInfo = pIONodeInfo;

	errLog = GetIONodeConfigurationFromSDFile();
	if (errLog == 0)
	{
		printf("Read IONode config file failed\n"); 
	}
	InitCanBpThread();




	/*------创建modbus从站处理线程-----------------*/
	printf("Start create ModbusProcess pthread!\n");

	errLog = pthread_create(&threadID_ModbusProcess,		/*modbusTcp*/
		NULL,
		(void* (*)(void*))MbProcessThread,	/**/
		NULL);  		/**/
	if(errLog!=0)
	{
		perror("thread ModbusProcess");
	}
	else
	{
		printf("create Modbusprocess thread OK!\n");
		pthread_detach(threadID_ModbusProcess); // 线程分离，结束时自动回收资源
	}

	/*------创建modbusTcp从站线程-----------------*/
	printf("Start create ModbusTcp pthread!\n");
	errLog = pthread_create(&threadID_ModbusTcp,		/*modbusTcp*/
							NULL,
							(void* (*)(void*))MbTcpServerListen,	/**/
							(void *) NULL);  		/**/
	if(errLog!=0)
	{
		perror("thread ModbusTcp");
	}
	else
	{
		printf("create ModbusTcp thread OK!\n");
		pthread_detach(threadID_ModbusTcp); // 线程分离，结束时自动回收资源
	}


	/***************************创建SPI线程***************************/
	//printf("Start cretae SPI test thread...... ");
	//errLog = pthread_create(&threadID_SPITest,NULL, (void* (*)(void*))spiTest,NULL);
	//if (errLog != 0)
	//{
 //   	perror("thread SPITest");
	//}
	//else
	//{
	//	printf("create SPITest thread OK!\n");
	//	pthread_detach(threadID_SPITest); // 线程分离，结束时自动回收资源
	//}

	/***************************创建GPIO线程***************************/
	//printf("Start cretae GPIO test thread...... ");
	//errLog = pthread_create(&threadID_GPIOTest, NULL, (void* (*)(void*))gpioTest, NULL);
	//if (errLog != 0)
	//{
	//	perror("thread gpioTest");
	//}
	//else
	//{
	//	printf("create gpioTest thread OK!\n");
	//	pthread_detach(threadID_GPIOTest); // 线程分离，结束时自动回收资源
	//}


/*------创建modbus主站测试程序-----------------*/
	//printf("Start create ModbusMasterTest pthread!\n");

	//errLog = pthread_create(&threadID_ModbusMasterTest,		/*modbusTcp*/
	//	NULL,
	//	(void* (*)(void*))MbMasterTest,	/**/
	//	NULL);  		/**/
	//if (errLog != 0)
	//{
	//	perror("thread MbMasterTest");
	//}
	//else
	//{
	//	printf("create MbMasterTest thread OK!\n");
	//	pthread_detach(threadID_ModbusMasterTest); // 线程分离，结束时自动回收资源
	//}


	//printf("Start create CanBackplaneTest pthread!\n");

	//errLog = pthread_create(&threadID_ModbusMasterTest,		/*modbusTcp*/
	//	NULL,
	//	(void* (*)(void*))MbMasterTest,	/**/
	//	NULL);  		/**/
	//if (errLog != 0)
	//{
	//	perror("thread MbMasterTest");
	//}
	//else
	//{
	//	printf("create MbMasterTest thread OK!\n");
	//	pthread_detach(threadID_ModbusMasterTest); // 线程分离，结束时自动回收资源
	//}
	

	//display();
	/*****************************************CAN点阵***************************************************/
	/*
	sleep(1);
	//j为标志位
	int j = 0;
	uint16_t tmp[1];
	while (1)
	{
		if (j == 0)
		{
			j = 1;
			DO_OutputU16_Force(0, 0xffff);
		}
		else
		{
			j = 0;
			DO_OutputU16_Force(0, 0x0000);
		}
	//此处为什么要有for循环？
		for (int i = 0; i < 100; i++)
		{
			tmp[0] = (uint16_t)GetDataByAddressU32(0, IOInOutType_DI);
			DO_OutputU16_Force(2, tmp[0]);
		    usleep(10000);
		}
	}
	*/
	/******************************************************************************************/
	
	uint8_t *pBuffRead =NULL;
	uint8_t *pBuffWrite = NULL;
	pBuffRead = (uint8_t *)malloc(255);
	pBuffWrite = (uint8_t *)malloc(255);
	if ((pBuffRead == NULL) || (pBuffWrite == NULL))
	{
		printf("main i2c mem malloc failed!\n");
		return 0;
	}

	for (uint32_t i = 0; i < 255; i++)
	{
		pBuffWrite[i] = i;
	}

	int errlog = 0;
	errlog = Bsp_M24lr26e_Read(0, pBuffRead, 255);
	if (errlog<0)
	{
		printf("i2c read failed!\n");
		free(pBuffWrite);
		free(pBuffRead);
		return 0;
	}

	sleep(1);
	errlog = Bsp_M24lr26e_Write(0, pBuffWrite, 255);
	if(errlog<0)
	{
		printf("i2c write failed!\n");
		free(pBuffWrite);
		free(pBuffRead);
		return 0;
	}
	sleep(1);
	errlog = Bsp_M24lr26e_Read(0, pBuffRead, 255);
	if (errlog<0)
	{
		printf("i2c read failed!\n");
		free(pBuffWrite);
		free(pBuffRead);
		return 0;
	}
	sleep(1);
	free(pBuffWrite);
	free(pBuffRead);
	return 0;
}



void ExitProgram(void)
{
	DeleteMsgQueue();
	if (sigaction(SIGINT, &struOldAct, NULL) == -1)
	{
		printf("[ERRORS] sigaction is failed!\n");
	}
}

void DeleteMsgQueue(void)
{
	msgctl(msgQId,IPC_RMID,NULL);
}

void CaptureQuitSignal(int nSignal)
{
	kill(0,SIGTERM);
	/*删除消息队列*/
	DeleteMsgQueue();

	/*恢复信号设置*/
	if(sigaction(SIGINT,&struOldAct,NULL)==-1)
	{
		printf("[ERRORS] sigaction is failed!\n");
	}
	exit(0);
}


