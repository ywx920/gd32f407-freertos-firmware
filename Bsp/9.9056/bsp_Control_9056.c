#include "bsp_Control_9056.h"
//#include "moni_iic.h"
#include "delay.h"

static i2c_handle_t bl24c02_handle=
{
	.i2c_sda.i2c_gpio_port=GPIOB,
	.i2c_sda.i2c_gpio_pin=GPIO_PIN_7,
	.i2c_scl.i2c_gpio_port=GPIOB,
	.i2c_scl.i2c_gpio_pin=GPIO_PIN_6,
};

u8 bl24c_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);

	gpio_init(bl24c02_handle.i2c_sda.i2c_gpio_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,\
		bl24c02_handle.i2c_sda.i2c_gpio_pin);
	gpio_init(bl24c02_handle.i2c_scl.i2c_gpio_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,\
		bl24c02_handle.i2c_scl.i2c_gpio_pin);
	
	return 0;
}


//在bl24c指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 bl24c_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    i2c_start(bl24c02_handle);  
	i2c_write_byte(0XA0+((ReadAddr/256)<<1),bl24c02_handle);   //发送器件地址0XA0,写数据 	   
	i2c_wait_ack(bl24c02_handle); 
    i2c_write_byte(ReadAddr%256,bl24c02_handle);   //发送低地址
	i2c_wait_ack(bl24c02_handle);	    
	i2c_start(bl24c02_handle);  	 	   
	i2c_write_byte(0XA1,bl24c02_handle);           //进入接收模式			   
	i2c_wait_ack(bl24c02_handle);	 
    temp=i2c_read_byte(0,bl24c02_handle);		   
    i2c_stop(bl24c02_handle);//产生一个停止条件	    
	return temp;
}
//在bl24c指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void bl24c_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    i2c_start(bl24c02_handle);  
	i2c_write_byte(0XA0+((WriteAddr/256)<<1),bl24c02_handle);   //发送器件地址0XA0,写数据 	 
	i2c_wait_ack(bl24c02_handle);	   
    i2c_write_byte(WriteAddr%256,bl24c02_handle);   //发送低地址
	i2c_wait_ack(bl24c02_handle); 	 										  		   
	i2c_write_byte(DataToWrite,bl24c02_handle);     //发送字节							   
	i2c_wait_ack(bl24c02_handle);  		    	   
    i2c_stop(bl24c02_handle);//产生一个停止条件 
	HAL_Delay(1);	 
}
//在bl24c里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void bl24c_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		bl24c_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在bl24c里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 bl24c_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=bl24c_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//检查bl24c是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 bl24c_Check(void)
{
	u8 temp;
	temp=bl24c_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		bl24c_WriteOneByte(255,0X55);
	    temp=bl24c_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//在bl24c里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void bl24c_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=bl24c_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在bl24c里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void bl24c_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		bl24c_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}



