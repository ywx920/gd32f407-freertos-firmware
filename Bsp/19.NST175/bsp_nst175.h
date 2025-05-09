#ifndef _BSP_NST175_H_
#define _BSP_NST175_H_

#include "sys_config.h"


#define TMP1_WRITE_ADDR 0x90  //0X48 0
#define TMP1_READ_ADDR 0x91		//0X48 1

#define TMP2_WRITE_ADDR 0x92  //0X49 0
#define TMP2_READ_ADDR 0x93		//0X49 1


#define TMP_REG_ADDR 0x00
#define TMP_CFIG_ADDR 0x01
#define TMP_TLOW_ADDR 0x02
#define TMP_THIGH_ADDR 0x03


void nst175_init(void);

uint16_t get_nst175_temperature(void);
void TempTask(void* param);

#endif //_BSP_NST175_H_
