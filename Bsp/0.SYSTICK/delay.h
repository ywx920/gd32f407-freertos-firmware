#ifndef _DELAY_H_
#define _DELAY_H_

#include "main.h"


typedef enum
{
    TICK_FREQ_1HZ          = 1000U,
    TICK_FREQ_10HZ         = 100U,
    TICK_FREQ_100HZ        = 10U,
    TICK_FREQ_1KHZ         = 1U,
    TICK_FREQ_DEFAULT      = TICK_FREQ_1KHZ
}TickFreqTypeDef;


void SYS_INIT(uint16_t SYSCLK);

void delay_us(uint32_t nus);
void delay_ms(uint16_t nms);


void HAL_Delay(uint32_t Delay);

#endif //_DELAY_H_
