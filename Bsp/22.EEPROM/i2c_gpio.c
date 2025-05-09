#include "i2c_gpio.h"

void EEPROM_I2C_CONFIG(void)
{
	rcu_periph_clock_enable(RCU_GPIOF);
	gpio_init(EEPROM_GPIO_PORT_I2C, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, EEPROM_I2C_SCL_PIN);
	gpio_init(EEPROM_GPIO_PORT_I2C, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, EEPROM_I2C_SDA_PIN);

	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	eeprom_i2c_Stop();
}

static void eeprom_i2c_Delay(void)
{
	uint8_t i;
	for (i = 0; i < 10; i++)
	{
	}
}

void eeprom_i2c_Start(void)
{
	EEPROM_SDA_OUT();
	delay_us(2);
	EEPROM_I2C_SCL_1();
	EEPROM_I2C_SDA_1();
	delay_us(2);
	EEPROM_I2C_SDA_0();
	delay_us(2);
	EEPROM_I2C_SCL_0();
	delay_us(2);
}

void eeprom_i2c_Stop(void)
{
	EEPROM_SDA_OUT();
	EEPROM_I2C_SDA_0();
	EEPROM_I2C_SCL_1();
	delay_us(2);
	EEPROM_I2C_SDA_1();
	delay_us(2);
}

void eeprom_i2c_Ack(void)
{
	EEPROM_I2C_SCL_0();
	EEPROM_SDA_OUT();
	delay_us(2);
	EEPROM_I2C_SDA_0();
	delay_us(2);
	EEPROM_I2C_SCL_1();
	delay_us(2);
	EEPROM_I2C_SCL_0();
	delay_us(2);
	EEPROM_I2C_SDA_1();
	delay_us(2);
}

void eeprom_i2c_NAcK(void)
{
	EEPROM_SDA_OUT();
	EEPROM_I2C_SDA_1();
	delay_us(2);
	EEPROM_I2C_SCL_1();
	delay_us(2);
	EEPROM_I2C_SCL_0();
	delay_us(2);
}

uint8_t eeprom_i2c_WaitAck(void)
{
	uint8_t ret;
	u16 ucErrTime=0;
	EEPROM_SDA_IN();
	EEPROM_I2C_SDA_1();
	delay_us(2);
	EEPROM_I2C_SCL_1();
	delay_us(2);
	while (EEPROM_I2C_SDA_READ())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			eeprom_i2c_Stop();
			return 1;
		}
	}
	// delay_us(2);
	EEPROM_I2C_SCL_0();
	// delay_us(2);
	return 0;
}

void eeprom_i2c_SendByte(uint8_t data)
{
	uint8_t i;
	EEPROM_SDA_OUT();
	// delay_us(2);
	// EEPROM_I2C_SCL_0();
	for (i = 0; i < 8; i++)
	{
		if (data & 0x80)		// 0X80:1000 0000 让要写入的数据一位一位的与 0X80 相与
		{						// 下面的 data <<= 1;直到整个数据都与其相与完毕。
			EEPROM_I2C_SDA_1(); // 相与：写入1的话返回1，写入0的话返回0
		}
		else
		{
			EEPROM_I2C_SDA_0();
		}
		delay_us(2);
		EEPROM_I2C_SCL_1();
		delay_us(2);
		EEPROM_I2C_SCL_0(); // SCL产生时钟
		delay_us(2);

		if (i == 7)
		{
			EEPROM_I2C_SDA_1(); // 释放控制权
			delay_us(2);
		}
		data <<= 1;
	}
}


uint8_t eeprom_i2c_ReadByte(void)
{
	uint8_t value = 0;
	uint8_t i;
	EEPROM_SDA_IN();
	for (i = 0; i < 8; i++)
	{
		value = value << 1;
		EEPROM_I2C_SCL_1();
		delay_us(2);
		if (EEPROM_I2C_SDA_READ())
		{
			// value++;
			value |= 0x01;
		}
		EEPROM_I2C_SCL_0();
		delay_us(2);
	}
	return value;
}

uint8_t eeprom_i2c_CheckDevice(uint8_t Address)
{
	uint8_t ucACK;
	EEPROM_I2C_CONFIG();
	eeprom_i2c_Start();
	eeprom_i2c_SendByte(Address | EEPROM_I2C_WR);
	ucACK = eeprom_i2c_WaitAck();
	eeprom_i2c_Stop();
	return ucACK;
}
