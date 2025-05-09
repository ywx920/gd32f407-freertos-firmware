#ifndef _BSP_NCT210_H_
#define _BSP_NCT210_H_

#include "main.h"


void nct210_init(void);
void nct210_write(uint8_t reg,uint8_t data);
uint8_t nct210_read(uint8_t reg);
uint8_t get_nct210_temperature(void);

#endif //_BSP_NCT210_H_
