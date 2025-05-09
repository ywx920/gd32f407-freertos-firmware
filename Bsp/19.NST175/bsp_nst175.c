/**************************************************************
 * 版权    : XXX @2024
 * 文件名  : bsp_nst175.c
 * 文件说明: nst175
 * 创建人  : leo 2024-04-10
 * 修改时间: 
 * ***********************************************************/
#include "bsp_nst175.h"
// #include "simulate_iic.h"
// #include "main.h"

static i2c_handle_t nst175_handle=
{
	.i2c_sda.i2c_gpio_port=GPIOF,
	.i2c_sda.i2c_gpio_pin=GPIO_PIN_3,
	.i2c_scl.i2c_gpio_port=GPIOF,
	.i2c_scl.i2c_gpio_pin=GPIO_PIN_4,
};

// static i2c_handle_t eeprom_handle=
// {
// 	.i2c_sda.i2c_gpio_port=GPIOF,
// 	.i2c_sda.i2c_gpio_pin=GPIO_PIN_11,
// 	.i2c_scl.i2c_gpio_port=GPIOF,
// 	.i2c_scl.i2c_gpio_pin=GPIO_PIN_12,
// };


void nst175_write_register(i2c_handle_t i2c,u8 device_address,u8 register_address,u8 data)
{
	i2c_start(i2c);
	i2c_write_byte(device_address,i2c);
	while(i2c_wait_ack(i2c));
	i2c_write_byte(register_address,i2c);
	while(i2c_wait_ack(i2c));
	i2c_write_byte(data,i2c);
	while(i2c_wait_ack(i2c));
	i2c_stop(i2c);
	delay_ms(1);
}


void nst175_init(void)
{
	/***时钟初始化***/	
	rcu_periph_clock_enable(RCU_GPIOF);
	/***引脚初始化***/
	gpio_init(nst175_handle.i2c_sda.i2c_gpio_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, nst175_handle.i2c_sda.i2c_gpio_pin);
	gpio_init(nst175_handle.i2c_scl.i2c_gpio_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, nst175_handle.i2c_scl.i2c_gpio_pin);
	
	IIC_SDA_H(nst175_handle);
	IIC_SCL_H(nst175_handle);
	/***配置温度寄存器***/
	nst175_write_register(nst175_handle, TMP1_WRITE_ADDR, TMP_CFIG_ADDR, 0x60);

	// gpio_init(eeprom_handle.i2c_sda.i2c_gpio_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, eeprom_handle.i2c_sda.i2c_gpio_pin);
	// gpio_init(eeprom_handle.i2c_scl.i2c_gpio_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, eeprom_handle.i2c_scl.i2c_gpio_pin);
	
	// IIC_SDA_H(eeprom_handle); 
	// IIC_SCL_H(eeprom_handle);


	/****
	i2c_start(nst175_handle);
	i2c_write_byte(TMP1_WRITE_ADDR,nst175_handle);
	while(i2c_wait_ack(nst175_handle));
	i2c_write_byte(TMP_CFIG_ADDR,nst175_handle);
	while(i2c_wait_ack(nst175_handle));
	i2c_write_byte(0x60,nst175_handle);
	while(i2c_wait_ack(nst175_handle));
	i2c_stop(nst175_handle);
	
	i2c_start(nst175_handle);
	i2c_write_byte(TMP1_WRITE_ADDR,nst175_handle);
	while(i2c_wait_ack(nst175_handle));
	i2c_write_byte(TMP_REG_ADDR,nst175_handle);
	while(i2c_wait_ack(nst175_handle));
	i2c_stop(nst175_handle);
	****/
}

uint16_t get_nst175_temperature(void)
{
	uint8_t NST175[2] = {0};
	uint16_t Temperature = 0;
	
	i2c_start(nst175_handle);
	i2c_write_byte(TMP1_READ_ADDR,nst175_handle);
	if(i2c_wait_ack(nst175_handle)) 
	{
			return 0;
	}

	
	NST175[1] = i2c_read_byte(1,nst175_handle);
	NST175[0] = i2c_read_byte(0,nst175_handle);
	Temperature = ((uint16_t)NST175[1] << 8) | ((uint16_t)NST175[0]);
	Temperature = Temperature >> 4;
	
	
	// if (NST175[1] > 0x7f) 
	// {
	// 	Temperature = ~Temperature + 1; 
	// }

	if ((Temperature & 0x8000) == 0x8000)					   // 负数取反加一
	{
		Temperature = ~Temperature;
		Temperature += 1;
	}
	Temperature = Temperature * 0.625; // 转换
	return Temperature;
	
}


static float temp1=0;

void TempTask(void* param)
{
	// nst175_init();
	
	uint16_t temp_integer=0;//整数
	uint16_t temp_decimal=0;//小数
	
	char decimal[25]={0};
	temp_integer=get_nst175_temperature() / 10;
	temp_decimal=get_nst175_temperature() % 10;
	while(1)
	{

		vTaskDelay(1000);
		temp_integer=get_nst175_temperature() / 10;
		temp_decimal=get_nst175_temperature() % 10;

		sprintf(decimal,"%d.%d",temp_integer,temp_decimal);
		sscanf(decimal,"%f",&temp1);

	}
}

void temppritf()
{
	uint16_t temp_integer=0;//整数
	uint16_t temp_decimal=0;//小数
	
	char decimal[25]={0};
	temp_integer=get_nst175_temperature() / 10;
	temp_decimal=get_nst175_temperature() % 10;

	sprintf(decimal,"%d.%d",temp_integer,temp_decimal);
	sscanf(decimal,"%f",&temp1);
    printf("\r\ntemp1:%.1f °C\r\n", temp1);
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
temppritf, temppritf, printf temp vaule);



// uint8_t AA24x_ReadOneByte(uint8_t addr)
// {
//     uint8_t temp;
//     i2c_start(eeprom_handle); 
//     i2c_write_byte(0xA0, eeprom_handle);//1010000
//     i2c_wait_ack(eeprom_handle);
//     i2c_write_byte(addr, eeprom_handle);
//     i2c_wait_ack(eeprom_handle);
//     i2c_start(eeprom_handle);
//     i2c_write_byte(0xA1, eeprom_handle);//10100001
//     i2c_wait_ack(eeprom_handle);
//     temp = i2c_read_byte(0, eeprom_handle); 


//     i2c_nack(eeprom_handle);
//     i2c_stop(eeprom_handle); 
//     return temp; 
// }

// void AA24x_WriteOneByte(uint8_t addr,uint8_t dt)
// {
// 	printf("write:%2X\r\n", dt);
//     i2c_start(eeprom_handle);
//     i2c_write_byte(0xA0, eeprom_handle);
//     i2c_wait_ack(eeprom_handle);
//     i2c_write_byte(addr, eeprom_handle); 
//     i2c_wait_ack(eeprom_handle);
//     i2c_write_byte(dt, eeprom_handle);
//     i2c_wait_ack(eeprom_handle);
//     i2c_stop(eeprom_handle);
//     delay_ms(10);
// }


// uint8_t ee_Test(void)
// {
// 	for (size_t i = 0; i < 200; i++)
// 	{
// 		/* code */
// 	}
	
// 	AA24x_WriteOneByte(0, i);
// 	printf("read:%2X\r\n", AA24x_ReadOneByte(0));

// }


// SHELL_EXPORT_CMD(
//     SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
//     ee_Test, ee_Test, eeprom_test vaule);
