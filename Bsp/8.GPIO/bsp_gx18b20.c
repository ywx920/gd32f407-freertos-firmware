#include "bsp_gx18b20.h"
#include "delay.h"
#include <stdio.h>
#include "shell_port.h"
//static short ft1temp=0;
//static short ft2temp=0;

//复位gx18b20
void gx18b20_Rst(uint32_t gpio_periph,uint32_t pin)	   
{                 
	gx18b20_IO_OUT(gpio_periph,pin);   //设置为输出
	gx18b20_DQ_OUT_L(gpio_periph,pin);  	//拉低DQ
	delay_us(750);      //拉低750us
	gx18b20_DQ_OUT_H(gpio_periph,pin);  	//DQ=1 
	delay_us(15);       //15US
}

//等待gx18b20的回应
//返回1:未检测到gx18b20的存在
//返回0:存在
uint8_t gx18b20_Check(uint32_t gpio_periph,uint32_t pin) 	   
{   
	uint8_t retry=0;
	gx18b20_IO_IN(gpio_periph,pin);    //设置为输入
    while ((gx18b20_DQ_IN(gpio_periph,pin))&&(retry<200))
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while ((!gx18b20_DQ_IN(gpio_periph,pin))&&(retry<240))
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}

//从gx18b20读取一个位
//返回值：1/0
uint8_t gx18b20_Read_Bit(uint32_t gpio_periph,uint32_t pin) 
{
	uint8_t data;
	gx18b20_IO_OUT(gpio_periph,pin);   //设置为输出
	gx18b20_DQ_OUT_L(gpio_periph,pin); 
	delay_us(2);
	gx18b20_DQ_OUT_H(gpio_periph,pin); 
	gx18b20_IO_IN(gpio_periph,pin);    //设置为输入
	delay_us(12);
	if(gx18b20_DQ_IN(gpio_periph,pin))data=1;
	else data=0;	 
	delay_us(50);           
	return data;
}

//从gx18b20读取一个字节
//返回值：读到的数据
uint8_t gx18b20_Read_Byte(uint32_t gpio_periph,uint32_t pin)   
{        
	uint8_t i,j,dat;
	dat=0;
	for (i=1;i<=8;i++) 
	{
        j=gx18b20_Read_Bit(gpio_periph,pin);
        dat=(j<<7)|(dat>>1);
    }						    
	return dat;
}

//写一个字节到gx18b20
//dat：要写入的字节
void gx18b20_Write_Byte(uint8_t dat,uint32_t gpio_periph,uint32_t pin)     
 {             
    uint8_t j;
    uint8_t testb;
    gx18b20_IO_OUT(gpio_periph,pin);     //设置为输出
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if(testb)       // 写1
        {
            gx18b20_DQ_OUT_L(gpio_periph,pin);
            delay_us(2);                            
            gx18b20_DQ_OUT_H(gpio_periph,pin);
            delay_us(60);             
        }
        else            //写0
        {
            gx18b20_DQ_OUT_L(gpio_periph,pin);
            delay_us(60);             
            gx18b20_DQ_OUT_H(gpio_periph,pin);
            delay_us(2);                          
        }
    }
}
 
//开始温度转换
void gx18b20_Start(uint32_t gpio_periph,uint32_t pin)
{   						               
    gx18b20_Rst(gpio_periph,pin);	   
    gx18b20_Check(gpio_periph,pin);	 
    gx18b20_Write_Byte(0xcc,gpio_periph,pin);// skip rom
    gx18b20_Write_Byte(0x44,gpio_periph,pin);// convert
}

/*  
    PD10---FT1 18B20
    PD9----FT2 18B20
    初始化gx18b20的IO口 DQ 同时检测DS的存在
    返回1:不存在
    返回0:存在   
*/ 	 
uint8_t gx18b20_Init(uint32_t gpio_periph,uint32_t pin)
{
	rcu_periph_clock_enable(RCU_GPIOD);
    
    gpio_init(gpio_periph, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, pin);
 
	gx18b20_Rst(gpio_periph,pin);
	return gx18b20_Check(gpio_periph,pin);
}

//从gx18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short gx18b20_Get_Temp(uint32_t gpio_periph,uint32_t pin)
{
    uint8_t temp;
    uint8_t TL,TH;
    short tem;
    gx18b20_Start(gpio_periph,pin);           //开始转换
    gx18b20_Rst(gpio_periph,pin);
    gx18b20_Check(gpio_periph,pin);	 
    gx18b20_Write_Byte(0xcc,gpio_periph,pin);   // skip rom
    gx18b20_Write_Byte(0xbe,gpio_periph,pin);   // convert	    
    TL=gx18b20_Read_Byte(gpio_periph,pin);     // LSB   
    TH=gx18b20_Read_Byte(gpio_periph,pin);     // MSB   
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//温度为负  
    }else temp=1;//温度为正	  	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(double)tem*0.625;//转换     
	if(temp)return tem; //返回温度值
	else return -tem;    
}

static 	uint8_t ft1initflag=0;
static	uint8_t ft2initflag=0;

void BmcTempInit(void)
{
	if (ft1initflag==0)
	{
	    if (gx18b20_Init(FT1_18B20_PORT,FT1_18B20_PIN)) 
			printf("FT1 gx18b20 check erro\r\n");
		else
			ft1initflag=1;
	}
	
	if (ft2initflag==0)
	{
	    if (gx18b20_Init(FT2_18B20_PORT,FT2_18B20_PIN)) 
			printf("FT2 gx18b20 check erro\r\n");
		else
			ft2initflag=1;
	}
}

static float temp_ft1=0;
 float temp_ft2=0;

void TempTask(void* param)
{
	BmcTempInit();
	uint16_t temp_integer=0;
	uint16_t temp_decimal=0;
	char decimal[25]={0};
	while(1)
	{
		temp_integer=gx18b20_Get_Temp(FT1_18B20_PORT,FT1_18B20_PIN)/10;
		temp_decimal=gx18b20_Get_Temp(FT1_18B20_PORT,FT1_18B20_PIN)%10;

		sprintf(decimal,"%d.%d",temp_integer,temp_decimal);
		sscanf(decimal,"%f",&temp_ft1);

		temp_integer=gx18b20_Get_Temp(FT2_18B20_PORT,FT2_18B20_PIN)/10;
		temp_decimal=gx18b20_Get_Temp(FT2_18B20_PORT,FT2_18B20_PIN)%10;

		sprintf(decimal,"%d.%d",temp_integer,temp_decimal);
		sscanf(decimal,"%f",&temp_ft2);
		// if (ft1initflag)
		// {
		// 	printf("\r\nFT1 temp:%d.%d°\r\n",gx18b20_Get_Temp(FT1_18B20_PORT,FT1_18B20_PIN)/10,gx18b20_Get_Temp(FT1_18B20_PORT,FT1_18B20_PIN)%10);
		// }
		// else
		// {
		// 	BmcTempInit();
		// }

		// if (ft2initflag)
		// {
		// 	printf("\r\nFT2 temp:%d.%d°\r\n",gx18b20_Get_Temp(FT2_18B20_PORT,FT2_18B20_PIN)/10,gx18b20_Get_Temp(FT2_18B20_PORT,FT2_18B20_PIN)%10);
		// }
		// else
		// {
		// 	BmcTempInit();
		// }
		
		vTaskDelay(2000);
	}
}

void temppritf()
{
    printf("\r\nft1temp:%f\r\n ft2temp:%fv\r\n", temp_ft1,temp_ft2);
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
temppritf, temppritf, printf temp vaule);


