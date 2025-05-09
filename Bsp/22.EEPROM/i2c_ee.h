#ifndef _I2C_EE_H
#define _I2C_EE_H


#include "sys_config.h"


#define EEPROM_DEV_ADDR			0xA0		/* 24xx02的设备地址 */
#define EEPROM_PAGE_SIZE		16			/* 24xx02的页面大小 */
#define EEPROM_SIZE				256			/* 24xx02总容量 */


uint8_t ee_Checkok(void);
uint8_t  ee_ReadByte( uint8_t *pReaddata,uint16_t Address,uint16_t num );
uint8_t  ee_WriteByte( uint8_t *Writepdata,uint16_t Address,uint16_t num );
uint8_t ee_WaitStandby(void);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t eeprom_read(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_Test(void) ;
#endif  /* _I2C_EE_H*/

