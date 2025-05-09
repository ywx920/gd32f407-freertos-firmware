#ifndef _I2C_GPIO_H
#define _I2C_GPIO_H


#include "sys_config.h"

#define EEPROM_I2C_WR	0		/* 写控制bit */
#define EEPROM_I2C_RD	1		/* 读控制bit */

#define EEPROM_GPIO_PORT_I2C         GPIOF
#define EEPROM_I2C_SCL_PIN           GPIO_PIN_11
#define EEPROM_I2C_SDA_PIN           GPIO_PIN_12

/*当 STM32 的 GPIO 配置成开漏输出模式时，它仍然可以通过读取
GPIO 的输入数据寄存器获取外部对引脚的输入电平，也就是说它同时具有浮空输入模式的
功能*/

#define EEPROM_SDA_OUT()   	gpio_init(EEPROM_GPIO_PORT_I2C, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, EEPROM_I2C_SDA_PIN)
#define EEPROM_SDA_IN()     gpio_init(EEPROM_GPIO_PORT_I2C, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PUPD_NONE, GPIO_OSPEED_50MHZ, EEPROM_I2C_SDA_PIN)


#define EEPROM_I2C_SCL_1()  gpio_bit_set(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SCL_PIN)		/* SCL = 1 */
#define EEPROM_I2C_SCL_0()  gpio_bit_reset(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SCL_PIN)		/* SCL = 0 */
	
#define EEPROM_I2C_SDA_1() gpio_bit_set(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SDA_PIN)		/* SDA = 1 */
#define EEPROM_I2C_SDA_0()   gpio_bit_reset(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SDA_PIN)		/* SDA = 0 */

#define EEPROM_I2C_SDA_READ()  gpio_input_bit_get(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SDA_PIN)	/* 读SDA口线状态 */

void EEPROM_I2C_CONFIG(void);
void eeprom_i2c_Start(void);
void eeprom_i2c_Stop(void);
void eeprom_i2c_Ack(void);
void eeprom_i2c_NAcK(void);
uint8_t eeprom_i2c_WaitAck(void);
void eeprom_i2c_SendByte(uint8_t data);
uint8_t eeprom_i2c_ReadByte(void);
uint8_t eeprom_i2c_CheckDevice(uint8_t Address);
#endif  /* _I2C_GPIO_H */

