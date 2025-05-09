#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

#include "sys_config.h"

void adcTask(void* param);
void adcinit();
int getAdc1(void);
int getAdc2(void);
int getAdc3(void);
int getAdc4(void);
int getAdc5(void);
int getAdc6(void);
int getAmpere(void);


#endif //_BSP_ADC_H_

