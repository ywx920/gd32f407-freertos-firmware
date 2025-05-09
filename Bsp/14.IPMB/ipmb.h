#ifndef _IPMB_H_
#define _IPMB_H_

#include "main.h"

#define DEF_APP_NETFN 0x06


void IPMBTask(void* param);

uint8_t test_iic(uint8_t DestAdd);
uint8_t checkSum(uint8_t vParA, uint8_t *mMsg, uint8_t vMsgLen);

#endif //_IPMB_H_
