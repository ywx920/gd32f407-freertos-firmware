#include "simulate_iic.h"
// #include "delay.h"

void i2c_start(i2c_handle_t i2c)
{
	SDA_OUT(i2c);     //sda线输出
	delay_us(2);
	IIC_SDA_H(i2c);	  	  
	IIC_SCL_H(i2c);
	delay_us(2);
 	IIC_SDA_L(i2c);//START:when CLK is high,DATA change form high to low 
	delay_us(2);
	IIC_SCL_L(i2c);//钳住I2C总线，准备发送或接收数据 
	delay_us(2);
}

void i2c_stop(i2c_handle_t i2c)
{
	SDA_OUT(i2c);//sda线输出
	IIC_SCL_L(i2c);
	IIC_SDA_L(i2c);//STOP:when CLK is high DATA change form low to high
 	delay_us(2);
	IIC_SCL_H(i2c);
	delay_us(2);
	IIC_SDA_H(i2c);//发送I2C总线结束信号
	delay_us(2);							   	
}

u8 i2c_wait_ack(i2c_handle_t i2c)
{
	u16 ucErrTime=0;
	SDA_IN(i2c);      //SDA设置为输入  
	IIC_SDA_H(i2c);delay_us(2);	   
	IIC_SCL_H(i2c);delay_us(2);	 
	while(READ_SDA(i2c))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			i2c_stop(i2c);
			return 1;
		}
	}
	delay_us(2);
	IIC_SCL_L(i2c);//时钟输出0 	
	delay_us(2);
	return 0;  
} 

void i2c_ack(i2c_handle_t i2c)
{
	IIC_SCL_L(i2c);
	SDA_OUT(i2c);
	IIC_SDA_L(i2c);
	delay_us(2);
	IIC_SCL_H(i2c);
	delay_us(2);
	IIC_SCL_L(i2c);
	delay_us(2);
	IIC_SDA_L(i2c);
}

void i2c_nack(i2c_handle_t i2c)
{
	IIC_SCL_L(i2c);
	SDA_OUT(i2c);
	IIC_SDA_H(i2c);
	delay_us(2);
	IIC_SCL_H(i2c);
	delay_us(2);
	IIC_SCL_L(i2c);
	delay_us(2);
	IIC_SDA_L(i2c);
}

u8 i2c_read_byte(uint8_t ack,i2c_handle_t i2c)
{
	u8 i,receive=0;
	SDA_IN(i2c);				// SDA input
	for (i=0;i<8;i++)
	{
		IIC_SCL_L(i2c);
		delay_us(2);
		IIC_SCL_H(i2c);
		receive<<=1;
		if (READ_SDA(i2c))
			receive++;
		delay_us(2);
		IIC_SCL_L(i2c);
		delay_us(2);
	}
	if (!ack)
		i2c_nack(i2c);
	else
		i2c_ack(i2c);
	return receive;
}


void i2c_write_byte(u8 txd,i2c_handle_t i2c)
{
	u8 t;
	SDA_OUT(i2c);
	IIC_SCL_L(i2c);
	for (t=0;t<8;t++)
	{
		if (((txd&0x80)>>7))
			IIC_SDA_H(i2c);
		else
			IIC_SDA_L(i2c);
		txd<<=1;
		delay_us(2);
		IIC_SCL_H(i2c);
		delay_us(2);
		IIC_SCL_L(i2c);
		delay_us(2);
	}
}
