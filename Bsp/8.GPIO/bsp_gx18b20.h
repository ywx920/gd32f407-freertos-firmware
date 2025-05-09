#ifndef _BSP_GX18B20_H
#define _BSP_GX18B20_H

#include "main.h"

#define gx18b20_IO_OUT(port,pin)    gpio_init(port, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, pin)
#define gx18b20_IO_IN(port,pin)     gpio_init(port, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, pin)

#define gx18b20_DQ_IN(port,pin)     gpio_input_bit_get(port,pin)
#define gx18b20_DQ_OUT_H(port,pin)  gpio_bit_set(port,pin)
#define gx18b20_DQ_OUT_L(port,pin)  gpio_bit_reset(port,pin)

#define FT1_18B20_PORT              GPIOD
#define FT2_18B20_PORT              GPIOD

#define FT1_18B20_PIN               GPIO_PIN_10
#define FT2_18B20_PIN               GPIO_PIN_9

short gx18b20_Get_Temp(uint32_t gpio_periph,uint32_t pin);
void BmcTempInit(void);
void TempTask(void* param);

#endif

