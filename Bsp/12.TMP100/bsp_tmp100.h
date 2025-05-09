#ifndef _BSP_TMP100_H_
#define _BSP_TMP100_H_

#include "main.h"


void tmp100_init(void);
void tmp100_write(uint8_t reg,uint8_t data);
uint16_t tmp100_read(uint8_t reg);
uint32_t get_tmp100_temperature(void);

#endif //_BSP_TMP100_H_
