/*************************************************************************
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.      *
* All rights reserved. All use of this software and documentation is     *
* subject to the License Agreement located at the end of this file below.*
**************************************************************************
* Description:                                                           *
* The following is a simple hello world program running MicroC/OS-II.The * 
* purpose of the design is to be a very simple application that just     *
* demonstrates MicroC/OS-II running on NIOS II.The design doesn't account*
* for issues such as checking system call return codes. etc.             *
*                                                                        *
* Requirements:                                                          *
*   -Supported Example Hardware Platforms                                *
*     Standard                                                           *
*     Full Featured                                                      *
*     Low Cost                                                           *
*   -Supported Development Boards                                        *
*     Nios II Development Board, Stratix II Edition                      *
*     Nios Development Board, Stratix Professional Edition               *
*     Nios Development Board, Stratix Edition                            *
*     Nios Development Board, Cyclone Edition                            *
*   -System Library Settings                                             *
*     RTOS Type - MicroC/OS-II                                           *
*     Periodic System Timer                                              *
*   -Know Issues                                                         *
*     If this design is run on the ISS, terminal output will take several*
*     minutes per iteration.                                             *
**************************************************************************/

#include "includes.h"
#include <ucos_ii.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <system.h>

/* Definition of Task Stacks */
#define TASK_STACKSIZE		1024
OS_STK		task0_stk[TASK_STACKSIZE];
OS_STK		task1_stk[TASK_STACKSIZE];
OS_STK		task2_stk[TASK_STACKSIZE];
OS_STK		task3_stk[TASK_STACKSIZE];
OS_STK		task4_stk[TASK_STACKSIZE];
OS_STK		task5_stk[TASK_STACKSIZE];
OS_STK		task6_stk[TASK_STACKSIZE];
OS_STK		task7_stk[TASK_STACKSIZE];
OS_STK		task8_stk[TASK_STACKSIZE];
OS_STK		task9_stk[TASK_STACKSIZE];
OS_STK		task10_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define TASK0_PRIORITY      0
#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2
#define TASK3_PRIORITY      3
#define TASK4_PRIORITY      4
#define TASK5_PRIORITY      5
#define TASK6_PRIORITY      6
#define TASK7_PRIORITY      7
#define TASK8_PRIORITY      8
#define TASK9_PRIORITY      9
#define TASK10_PRIORITY     10

/* Definition of I/O Addresses */
volatile int* InPort_COUNTER = (int*) 0x04009050; // MSB
volatile int* HEX0 = (int*) 0x040090e0; // HEX0
volatile int* HEX1 = (int*) 0x040090a0; // HEX1
volatile int* HEX2 = (int*) 0x04009080; // HEX2
volatile int* HEX3 = (int*) 0x04009070; // HEX3
volatile int* UART = (int*) 0x04009020; // UART
volatile int* LED0 = (int*) 0x040090d0; // LED0
volatile int* LED14 = (int*) 0x040090b0; // LED1 to LED4
volatile int* LED59 = (int*) 0x04009090; // LED5 to LED9
volatile int* servo = (int*) 0x04009060; // servo enable
volatile int* motor = (int*) 0x040090c0; // motor enable
volatile int* value = (int*) 0x04009040; // servo value

/* Definition of Queue Pointers */
OS_EVENT *QSemTask1;
OS_EVENT *QSemTask2;
OS_EVENT *QSemTask3;
OS_EVENT *QSemTask4;
OS_EVENT *QSemTask5;
OS_EVENT *QSemTask6;
OS_EVENT *QSemTask7;
OS_EVENT *QSemTask8;
OS_EVENT *QSemTask9;
OS_EVENT *QSemTask10;

/* Definition of Queue Tables */
#define N_RESOURCES		20
void *QMsgTblTask1[N_RESOURCES];
void *QMsgTblTask2[N_RESOURCES];
void *QMsgTblTask3[N_RESOURCES];
void *QMsgTblTask4[N_RESOURCES];
void *QMsgTblTask5[N_RESOURCES];
void *QMsgTblTask6[N_RESOURCES];
void *QMsgTblTask7[N_RESOURCES];
void *QMsgTblTask8[N_RESOURCES];
void *QMsgTblTask9[N_RESOURCES];
void *QMsgTblTask10[N_RESOURCES];

/* Definition of Task Duration (in ticks, 10 ms per tick) */
#define TASK1_TICKS      1000 // 10 s
#define TASK7_TICKS      300 // 3 s
#define TASK8_TICKS      300 // 3 s
#define TASK9_TICKS      1200 // 12 s

/* Definition of semaphore */
OS_EVENT *SemProtect;
#define N_SEM_PROTECT	1

/* Global variables */
int wafers[N_RESOURCES];
float totalTime;
float plStartTime;
float plEndTime;

/* Process 1: cleaning */
void task1(void* pdata)
{
	int* wafer;
	INT8U err;
	INT8U errSem;
	INT32U LEDStartTime;
	INT32U startTime;
	float taskTime;
	int HEX0_bits = 0b00000001;
	while(1)
	{
		// Take a wafer from process 1's queue table to process
		wafer = (int*) OSQPend(QSemTask1, 0, &err);
		if (err == OS_NO_ERR)
		{
			if (*(wafer) == wafers[0])
			{
				// Record the starting time of pipelining and of process
				startTime = OSTimeGet();
				plStartTime = startTime;

				// Write status to UART indicating the start of pipelining (START) and of process 1
				FILE* fp;
				fp = fopen(UART_0_NAME, "w");
				if (fp == NULL)
				{
					printf("START/Process 1 begin open file failed.\n");
				}
				fprintf(fp, "%d,1,0,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
				fclose(fp);
				printf("START\n");
			}
			else
			{
				// Record the starting time of process
				startTime = OSTimeGet();

				// Write status to UART indicating the start of process 1
				FILE* fp;
				fp = fopen(UART_0_NAME, "w");
				if (fp == NULL)
				{
					printf("Process 1 begin open file failed.\n");
				}
				fprintf(fp, "%d,1,0,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
				fclose(fp);
			}

			printf("Wafer %d: process 1 begin.\n", *(wafer));

			// Enable segment circling on HEX0 for 10 s
			LEDStartTime = OSTimeGet();
			while(OSTimeGet() - LEDStartTime < TASK1_TICKS)
			{
				*(HEX0) = 0b11111111 - HEX0_bits;
				if(HEX0_bits < 0b00100000)
				{
					HEX0_bits = 2*HEX0_bits;
				}
				else
				{
					HEX0_bits = 0b00000001;
				}
				OSTimeDlyHMSM(0, 0, 0, 500);
			}

			// Disable segment circling ON HEX0
			*(HEX0) = 0b11111111;

			// Delay for 5 s to avoid congestion at process 2
			OSTimeDlyHMSM(0, 0, 5, 0);

			OSSemPend(SemProtect, 0, &errSem);
			if (errSem == OS_NO_ERR)
			{
				// Update total time
				taskTime = (OSTimeGet() - startTime)/100.00f;
				totalTime += taskTime;

				OSSemPost(SemProtect);
			}

			// Write status to UART
			FILE* fp;
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 1 complete open file failed.\n");
			}
			printf("Wafer %d: process 1 complete. Time taken: %.0f s. CPU usage: %d%%.\n", *(wafer), taskTime, OSCPUUsage);
			fprintf(fp, "%d,1,1,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			// Pass the wafer to process 2's queue table
			OSQPost(QSemTask2, wafer);
		}
	}
}

/* Process 2: drying and prebaking */
void task2(void* pdata)
{
	int* wafer;
	INT8U err;
	INT8U errSem;
	INT32U startTime;
	float taskTime;
	while(1)
	{
		// Take a wafer from process 2's queue table to process
		wafer = (int*) OSQPend(QSemTask2, 0, &err);
		if (err == OS_NO_ERR)
		{
			// Record the starting time of process
			startTime = OSTimeGet();

			// Write status to UART indicating the start of process 2
			FILE* fp;
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 2 begin open file failed.\n");
			}
			fprintf(fp, "%d,2,0,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			printf("Wafer %d: process 2 begin.\n", *(wafer));

			// Turn LED0 on for 13 s
			*(LED0) = 1;
			OSTimeDlyHMSM(0, 0, 13, 0);

			// Turn LED0 off
			*(LED0) = 0;

			OSSemPend(SemProtect, 0, &errSem);
			if (errSem == OS_NO_ERR)
			{
				// Update total time
				taskTime = (OSTimeGet() - startTime)/100.00f;
				totalTime += taskTime;

				OSSemPost(SemProtect);
			}

			// Write status to UART
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 2 complete open file failed.\n");
			}
			printf("Wafer %d: process 2 complete. Time taken: %.0f s. CPU usage: %d%%.\n", *(wafer), taskTime, OSCPUUsage);
			fprintf(fp, "%d,2,1,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			// Pass the wafer to process 3's queue table
			OSQPost(QSemTask3, wafer);
		}
	}
}

/* Process 3: photoresist coating */
void task3(void* pdata)
{
	int* wafer;
	INT8U err;
	INT8U errSem;
	INT32U startTime;
	float taskTime;
	while(1)
	{
		// Take a wafer from process 3's queue table to process
		wafer = (int*) OSQPend(QSemTask3, 0, &err);
		if (err == OS_NO_ERR)
		{
			// Record the starting time of process
			startTime = OSTimeGet();

			// Write status to UART indicating the start of process 3
			FILE* fp;
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 3 begin open file failed.\n");
			}
			fprintf(fp, "%d,3,0,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			printf("Wafer %d: process 3 begin.\n", *(wafer));

			// Enable stepper motor for 8 s
			*(motor) = 1;
			OSTimeDlyHMSM(0, 0, 8, 0);

			// Disable stepper motor
			*(motor) = 0;

			OSSemPend(SemProtect, 0, &errSem);
			if (errSem == OS_NO_ERR)
			{
				// Update total time
				taskTime = (OSTimeGet() - startTime)/100.00f;
				totalTime += taskTime;

				OSSemPost(SemProtect);
			}

			// Write status to UART
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 3 complete open file failed.\n");
			}
			printf("Wafer %d: process 3 complete. Time taken: %.0f s. CPU usage: %d%%.\n", *(wafer), taskTime, OSCPUUsage);
			fprintf(fp, "%d,3,1,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			// Pass the wafer to process 4's queue table
			OSQPost(QSemTask4, wafer);
		}
	}
}

/* Process 4: pre-exposure baking */
void task4(void* pdata)
{
	int* wafer;
	INT8U err;
	INT8U errSem;
	INT32U startTime;
	float taskTime;
	while(1)
	{
		// Take a wafer from process 4's queue table to process
		wafer = (int*) OSQPend(QSemTask4, 0, &err);
		if (err == OS_NO_ERR)
		{
			// Record the starting time of process
			startTime = OSTimeGet();

			// Write status to UART indicating the start of process 4
			FILE* fp;
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 4 begin open file failed.\n");
			}
			fprintf(fp, "%d,4,0,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			printf("Wafer %d: process 4 begin.\n", *(wafer));

			// Enable sequential lighting of LED1, LED2, LED3, and LED4 for 5 s
			*(LED14) = 0b0001;
			OSTimeDlyHMSM(0, 0, 1, 250);
			*(LED14) = 0b0011;
			OSTimeDlyHMSM(0, 0, 1, 250);
			*(LED14) = 0b0111;
			OSTimeDlyHMSM(0, 0, 1, 250);
			*(LED14) = 0b1111;
			OSTimeDlyHMSM(0, 0, 1, 250);

			// Disable sequential lighting of LED1, LED2, LED3, and LED4
			*(LED14) = 0b0000;

			OSSemPend(SemProtect, 0, &errSem);
			if (errSem == OS_NO_ERR)
			{
				// Update total time
				taskTime = (OSTimeGet() - startTime)/100.00f;
				totalTime += taskTime;

				OSSemPost(SemProtect);
			}

			// Write status to UART
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 4 complete open file failed.\n");
			}
			printf("Wafer %d: process 4 complete. Time taken: %.0f s. CPU usage: %d%%.\n", *(wafer), taskTime, OSCPUUsage);
			fprintf(fp, "%d,4,1,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			// Pass the wafer to process 5's queue table
			OSQPost(QSemTask5, wafer);
		}
	}
}

/* Process 5: lithography exposure */
void task5(void* pdata)
{
	int* wafer;
	INT8U err;
	INT8U errSem;
	INT32U startTime;
	float taskTime;
	while(1)
	{
		// Take a wafer from process 5's queue table to process
		wafer = (int*) OSQPend(QSemTask5, 0, &err);
		if (err == OS_NO_ERR)
		{
			// Record the starting time of process
			startTime = OSTimeGet();

			// Write status to UART indicating the start of process 5
			FILE* fp;
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 5 begin open file failed.\n");
			}
			fprintf(fp, "%d,5,0,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			printf("Wafer %d: process 5 begin.\n", *(wafer));

			// Turn HEX1 on for 5 s
			*(HEX1) = 0b10000000;
			OSTimeDlyHMSM(0, 0, 5, 0);

			// Turn HEX1 off
			*(HEX1) = 0b11111111;

			OSSemPend(SemProtect, 0, &errSem);
			if (errSem == OS_NO_ERR)
			{
				// Update total time
				taskTime = (OSTimeGet() - startTime)/100.00f;
				totalTime += taskTime;

				OSSemPost(SemProtect);
			}

			// Write status to UART
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 5 complete open file failed.\n");
			}
			printf("Wafer %d: process 5 complete. Time taken: %.0f s. CPU usage: %d%%.\n", *(wafer), taskTime, OSCPUUsage);
			fprintf(fp, "%d,5,1,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			// Pass the wafer to process 6's queue table
			OSQPost(QSemTask6, wafer);
		}
	}
}

/* Process 6: post-exposure baking */
void task6(void* pdata)
{
	int* wafer;
	INT8U err;
	INT8U errSem;
	INT32U startTime;
	float taskTime;
	while(1)
	{
		// Take a wafer from process 6's queue table to process
		wafer = (int*) OSQPend(QSemTask6, 0, &err);
		if (err == OS_NO_ERR)
		{
			// Record the starting time of process
			startTime = OSTimeGet();

			// Write status to UART indicating the start of process 6
			FILE* fp;
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 6 begin open file failed.\n");
			}
			fprintf(fp, "%d,6,0,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			printf("Wafer %d: process 6 begin.\n", *(wafer));

			// Enable sequential lighting of LED5, LED6, LED7, LED8, and LED9 for 5 s
			*(LED59) = 0b00001;
			OSTimeDlyHMSM(0, 0, 1, 0);
			*(LED59) = 0b00011;
			OSTimeDlyHMSM(0, 0, 1, 0);
			*(LED59) = 0b00111;
			OSTimeDlyHMSM(0, 0, 1, 0);
			*(LED59) = 0b01111;
			OSTimeDlyHMSM(0, 0, 1, 0);
			*(LED59) = 0b11111;
			OSTimeDlyHMSM(0, 0, 1, 0);

			// Disable sequential lighting of LED5, LED6, LED7, LED8, and LED9
			*(LED59) = 0b00000;

			OSSemPend(SemProtect, 0, &errSem);
			if (errSem == OS_NO_ERR)
			{
				// Update total time
				taskTime = (OSTimeGet() - startTime)/100.00f;
				totalTime += taskTime;

				OSSemPost(SemProtect);
			}

			// Write status to UART
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 6 complete open file failed.\n");
			}
			printf("Wafer %d: process 6 complete. Time taken: %.0f s. CPU usage: %d%%.\n", *(wafer), taskTime, OSCPUUsage);
			fprintf(fp, "%d,6,1,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			// Pass the wafer to process 7's queue table
			OSQPost(QSemTask7, wafer);
		}
	}
}

/* Process 7: development */
void task7(void* pdata)
{
	int* wafer;
	INT8U err;
	INT8U errSem;
	INT32U LEDStartTime;
	INT32U startTime;
	float taskTime;
	int HEX2_bits = 0b00000001;
	while(1)
	{
		// Take a wafer from process 7's queue table to process
		wafer = (int*) OSQPend(QSemTask7, 0, &err);
		if (err == OS_NO_ERR)
		{
			// Record the starting time of process
			startTime = OSTimeGet();

			// Write status to UART indicating the start of process 7
			FILE* fp;
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 7 begin open file failed.\n");
			}
			fprintf(fp, "%d,7,0,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			printf("Wafer %d: process 7 begin.\n", *(wafer));

			// Enable segment circling on HEX2 for 3 s
			LEDStartTime = OSTimeGet();
			while((OSTimeGet() - LEDStartTime) < TASK7_TICKS)
			{
				*(HEX2) = 0b11111111 - HEX2_bits;
				if(HEX2_bits < 0b00100000)
				{
					HEX2_bits = 2*HEX2_bits;
				}
				else
				{
					HEX2_bits = 0b00000001;
				}
				OSTimeDlyHMSM(0, 0, 0, 500);
			}

			// Disable segment circling on HEX2
			*(HEX2) = 0b11111111;

			OSSemPend(SemProtect, 0, &errSem);
			if (errSem == OS_NO_ERR)
			{
				// Update total time
				taskTime = (OSTimeGet() - startTime)/100.00f;
				totalTime += taskTime;

				OSSemPost(SemProtect);
			}

			// Write status to UART
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 7 complete open file failed.\n");
			}
			printf("Wafer %d: process 7 complete. Time taken: %.0f s. CPU usage: %d%%.\n", *(wafer), taskTime, OSCPUUsage);
			fprintf(fp, "%d,7,1,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			// Pass the wafer to process 8's queue table
			OSQPost(QSemTask8, wafer);
		}
	}
}

/* Process 8: etching */
void task8(void* pdata)
{
	int* wafer;
	INT8U err;
	INT8U errSem;
	INT32U LEDStartTime;
	INT32U startTime;
	float taskTime;
	int HEX3_bits = 0b00000001;
	while(1)
	{
		// Take a wafer from process 8's queue table to process
		wafer = (int*) OSQPend(QSemTask8, 0, &err);
		if (err == OS_NO_ERR)
		{
			// Record the starting time of process
			startTime = OSTimeGet();

			// Write status to UART indicating the start of process 8
			FILE* fp;
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 8 begin open file failed.\n");
			}
			fprintf(fp, "%d,8,0,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			printf("Wafer %d: process 8 begin.\n", *(wafer));

			// Enable segment circling on HEX3 for 3 s
			LEDStartTime = OSTimeGet();
			while((OSTimeGet() - LEDStartTime) < TASK8_TICKS)
			{
				*(HEX3) = 0b11111111 - HEX3_bits;
				if(HEX3_bits < 0b00100000)
				{
					HEX3_bits = 2*HEX3_bits;
				}
				else
				{
					HEX3_bits = 0b00000001;
				}
				OSTimeDlyHMSM(0, 0, 0, 500);
			}

			// Disable segment circling on HEX3
			*(HEX3) = 0b11111111;

			OSSemPend(SemProtect, 0, &errSem);
			if (errSem == OS_NO_ERR)
			{
				// Update total time
				taskTime = (OSTimeGet() - startTime)/100.00f;
				totalTime += taskTime;

				OSSemPost(SemProtect);
			}

			// Write status to UART
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 8 complete open file failed.\n");
			}
			printf("Wafer %d: process 8 complete. Time taken: %.0f s. CPU usage: %d%%.\n", *(wafer), taskTime, OSCPUUsage);
			fprintf(fp, "%d,8,1,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			// Pass the wafer to process 9's queue table
			OSQPost(QSemTask9, wafer);
		}
	}
}

/* Process 9: photoresist cleaning and finalising */
void task9(void* pdata)
{
	int* wafer;
	INT8U err;
	INT8U errSem;
	INT32U servoStartTime;
	INT32U startTime;
	float taskTime;
	while(1)
	{
		// Take a wafer from process 9's queue table to process
		wafer = (int*) OSQPend(QSemTask9, 0, &err);
		if (err == OS_NO_ERR)
		{
			// Record the starting time of process
			startTime = OSTimeGet();

			// Write status to UART indicating the start of process 9
			FILE* fp;
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 9 begin open file failed.\n");
			}
			fprintf(fp, "%d,9,0,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			printf("Wafer %d: process 9 begin.\n", *(wafer));

			// Enable servo motor for 12 s
			*(value) = 0b0001001011;
			servoStartTime = OSTimeGet();
			while((OSTimeGet() - servoStartTime) < TASK9_TICKS)
			{
				*(servo) = 1;
				OSTimeDlyHMSM(0, 0, 0, 200);
				*(servo) = 0;
				OSTimeDlyHMSM(0, 0, 0, 800);
			}

			// Disable servo motor
			*(servo) = 0;
			*(value) = 0b0000000000;

			OSSemPend(SemProtect, 0, &errSem);
			if (errSem == OS_NO_ERR)
			{
				// Update total time
				if (*(wafer) == wafers[N_RESOURCES - 1])
				{
					// Record the end time of pipelining
					plEndTime = OSTimeGet();
					taskTime = (plEndTime - startTime)/100.00f;
				}
				else
				{
					taskTime = (OSTimeGet() - startTime)/100.00f;

				}
				totalTime += taskTime;

				OSSemPost(SemProtect);
			}

			// Write status to UART
			fp = fopen(UART_0_NAME, "w");
			if (fp == NULL)
			{
				printf("Process 9 complete open file failed.\n");
			}
			printf("Wafer %d: process 9 complete. Time taken: %.0f s. CPU usage: %d%%.\n", *(wafer), taskTime, OSCPUUsage);
			fprintf(fp, "%d,9,1,%d;", *(wafer), OSCPUUsage); // String format: wafer_ID,current_process,process_start/end,CPU_usage;
			fclose(fp);

			// Display IDLE on HEX0, HEX1, HEX2, and HEX3 when pipelining is complete
			if (*(wafer) == wafers[N_RESOURCES - 1])
			{
				*(HEX3) = 0b11111001; // I
				*(HEX2) = 0b10100001; // D
				*(HEX1) = 0b11000111; // L
				*(HEX0) = 0b10000110; // E

				printf("The average time taken to process 1 wafer (if pipelining was not implemented) would be: %.0f s.\n", (totalTime/(float) N_RESOURCES));
				printf("The average time taken to process 1 wafer (as a result of pipelining) is: %.0f s.\n", (plEndTime - plStartTime)/2000.00f); // [(end - start)/100]/20
				printf("The total time taken to process %d wafers is: %.0f s.\n", N_RESOURCES, (plEndTime - plStartTime)/100.00f);
			}

			// Pass the wafer to process 10's queue table
			OSQPost(QSemTask10, wafer);
		}
	}
}

/* Process 10: idle */
void task10(void* pdata)
{
	while(1)
	{
		OSTimeDlyHMSM(0, 25, 0, 0);
	}
}

/* taskStart initialises Statistic task and the 10 processes, and creates wafers */
void taskStart(void *pdata)
{
    // Initialise Statistic task to calculate CPU usage
    OSStatInit();
    OSTimeDlyHMSM(0, 0, 10, 0);

    // Initialise process 1
    OSTaskCreateExt(task1,
                    NULL,
                    (void *)&task1_stk[TASK_STACKSIZE-1],
                    TASK1_PRIORITY,
                    TASK1_PRIORITY,
                    task1_stk,
                    TASK_STACKSIZE,
                    NULL,
                    0);
    // QSemTask1 points to queue table QMsgTblTask1
    QSemTask1 = OSQCreate(&QMsgTblTask1[0], N_RESOURCES);

    // 'Create' wafer by posting wafer (message) into process 1's queue table
    for (int i = 0; i < N_RESOURCES; i++)
    {
    	wafers[i] = i + 1; // Assign wafer ID
        OSQPost(QSemTask1, &wafers[i]); // Post wafer into process 1's queue table
    }

    // Initialise process 2
    OSTaskCreateExt(task2,
                    NULL,
                    (void *)&task2_stk[TASK_STACKSIZE-1],
                    TASK2_PRIORITY,
                    TASK2_PRIORITY,
                    task2_stk,
                    TASK_STACKSIZE,
                    NULL,
                    0);
    // QSemTask2 points to queue table QMsgTblTask2
    QSemTask2 = OSQCreate(&QMsgTblTask2[0], N_RESOURCES);

    // Initialise process 3
    OSTaskCreateExt(task3,
                    NULL,
                    (void *)&task3_stk[TASK_STACKSIZE-1],
                    TASK3_PRIORITY,
                    TASK3_PRIORITY,
                    task3_stk,
                    TASK_STACKSIZE,
                    NULL,
                    0);
    QSemTask3 = OSQCreate(&QMsgTblTask3[0], N_RESOURCES);

    // Initialise process 4
    OSTaskCreateExt(task4,
                    NULL,
                    (void *)&task4_stk[TASK_STACKSIZE-1],
                    TASK4_PRIORITY,
                    TASK4_PRIORITY,
                    task4_stk,
                    TASK_STACKSIZE,
                    NULL,
                    0);
    QSemTask4 = OSQCreate(&QMsgTblTask4[0], N_RESOURCES);

    // Initialise process 5
    OSTaskCreateExt(task5,
                    NULL,
                    (void *)&task5_stk[TASK_STACKSIZE-1],
                    TASK5_PRIORITY,
                    TASK5_PRIORITY,
                    task5_stk,
                    TASK_STACKSIZE,
                    NULL,
                    0);
    QSemTask5 = OSQCreate(&QMsgTblTask5[0], N_RESOURCES);

    // Initialise process 6
    OSTaskCreateExt(task6,
                    NULL,
                    (void *)&task6_stk[TASK_STACKSIZE-1],
                    TASK6_PRIORITY,
                    TASK6_PRIORITY,
                    task6_stk,
                    TASK_STACKSIZE,
                    NULL,
                    0);
    QSemTask6 = OSQCreate(&QMsgTblTask6[0], N_RESOURCES);

    // Initialise process 7
    OSTaskCreateExt(task7,
                    NULL,
                    (void *)&task7_stk[TASK_STACKSIZE-1],
                    TASK7_PRIORITY,
                    TASK7_PRIORITY,
                    task7_stk,
                    TASK_STACKSIZE,
                    NULL,
                    0);
    QSemTask7 = OSQCreate(&QMsgTblTask7[0], N_RESOURCES);

    // Initialise process 8
    OSTaskCreateExt(task8,
                    NULL,
                    (void *)&task8_stk[TASK_STACKSIZE-1],
                    TASK8_PRIORITY,
                    TASK8_PRIORITY,
                    task8_stk,
                    TASK_STACKSIZE,
                    NULL,
                    0);
    QSemTask8 = OSQCreate(&QMsgTblTask8[0], N_RESOURCES);

    // Initialise process 9
    OSTaskCreateExt(task9,
                    NULL,
                    (void *)&task9_stk[TASK_STACKSIZE-1],
                    TASK9_PRIORITY,
                    TASK9_PRIORITY,
                    task9_stk,
                    TASK_STACKSIZE,
                    NULL,
                    0);
    QSemTask9 = OSQCreate(&QMsgTblTask9[0], N_RESOURCES);

    // Initialise process 10
    OSTaskCreateExt(task10,
                    NULL,
                    (void *)&task10_stk[TASK_STACKSIZE-1],
                    TASK10_PRIORITY,
                    TASK10_PRIORITY,
                    task10_stk,
                    TASK_STACKSIZE,
                    NULL,
                    0);
    QSemTask10 = OSQCreate(&QMsgTblTask10[0], N_RESOURCES);

    // Initialise semaphore for protection of totalTime global variable
    SemProtect = OSSemCreate(N_SEM_PROTECT);

    // Initially turn off all outputs
	*(HEX3) = 0b11111111;
	*(HEX2) = 0b11111111;
	*(HEX1) = 0b11111111;
	*(HEX0) = 0b11111111;
	*(motor) = 0;
	*(servo) = 0;

    while (1)
    {
    	printf("taskStart is still here!\n");
    	OSTimeDlyHMSM(0, 20, 0, 0);
    }
}

/* The main function creates taskStart */
int main(void)
{
	printf("MicroC/OS-II Licensing Terms\n");
	printf("============================\n");
	printf("Micrium\'s uC/OS-II is a real-time operating system (RTOS) available in source code.\n");
	printf("This is not open-source software.\n");
	printf("This RTOS can be used free of charge only for non-commercial purposes and academic projects,\n");
	printf("any other use of the code is subject to the terms of an end-user license agreement\n");
	printf("for more information please see the license files included in the BSP project or contact Micrium.\n");
	printf("Anyone planning to use a Micrium RTOS in a commercial product must purchase a commercial license\n");
	printf("from the owner of the software, Silicon Laboratories Inc.\n");
	printf("Licensing information is available at:\n");
	printf("Phone: +1 954-217-2036\n");
	printf("Email: sales@micrium.com\n");
	printf("URL: www.micrium.com\n\n\n");

	OSTaskCreateExt(taskStart,
                  	NULL,
					(void *)&task0_stk[TASK_STACKSIZE-1],
					TASK0_PRIORITY,
					TASK0_PRIORITY,
					task0_stk,
					TASK_STACKSIZE,
					NULL,
					0);

  OSStart();

  return 0;
}

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
******************************************************************************/
