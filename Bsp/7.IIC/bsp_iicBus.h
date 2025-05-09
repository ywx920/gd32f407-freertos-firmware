#ifndef _BSP_IICBUS_H_
#define _BSP_IICBUS_H_

#include "main.h"

void IICBUSTask(void* param);

uint16_t get_t210temp(void);
uint16_t get_t100temp(void);

#endif //_BSP_IICBUS_H_
