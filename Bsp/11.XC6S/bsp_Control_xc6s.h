#ifndef _BSP_CONTROL_XC6S_H_
#define _BSP_CONTROL_XC6S_H_

#include "main.h"

void uart3_init(uint32_t baudval);

void uart3Send_char(uint8_t ch);

void fan_level(uint8_t level);

void setfanMode(uint8_t mode);

uint8_t getFanMode(void);

#endif //_BSP_CONTROL_XC6S_H_
