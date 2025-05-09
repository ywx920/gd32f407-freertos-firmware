#ifndef _COMMCPU_H
#define _COMMCPU_H

#include "sys_config.h"

#define COMCPU_CPU1 USART1
#define COMCPU_CPU2 UART7

#define COMMCPU_PRO_HEAD       0x7e7e
#define COMMCPU_PRO_TAIL       0xfdfd

#define COMMCPU_PRO_HEAD_LEN   2
#define COMMCPU_PRO_TAIL_LEN   2

#define COMMCPU_MAXBUFF_SIZE   512

#define COMM_SERVER_ID 0x8000  //cpu主机
#define COMM_CLIENT_ID 0x7e7e   //板卡

uint16_t send_cpu1msg(uint8_t *buf,uint16_t mstrlen);
uint16_t recv_cpu1msg(uint8_t *buf);
uint16_t send_cpu2msg(uint8_t *buf,uint16_t mstrlen);
uint16_t recv_cpu2msg(uint8_t *buf);
void ComCpu1Task(void* param);
void ComCpu2Task(void* param);


#endif

