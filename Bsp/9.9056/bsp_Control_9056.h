#ifndef _BSP_BL24C02A_H
#define _BSP_BL24C02A_H

#include "main.h"

void bl24c_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);
u32 bl24c_ReadLenByte(u16 ReadAddr,u8 Len);
u8 bl24c_init(void);
u8 bl24c_Check(void);


#endif

