#ifndef _SIMULATE_IIC_H
#define _SIMULATE_IIC_H

#include "sys_config.h"

typedef struct i2c_gpio_handle_s
{
	u32 i2c_gpio_port;
	u32 i2c_gpio_pin;
}i2c_gpio_handle_t;

typedef struct i2c_handle_s
{
	i2c_gpio_handle_t i2c_sda;
	i2c_gpio_handle_t i2c_scl;
}i2c_handle_t;


#define SDA_OUT(i2chandle)    gpio_init(i2chandle.i2c_sda.i2c_gpio_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, i2chandle.i2c_sda.i2c_gpio_pin)
#define SDA_IN(i2chandle)     gpio_init(i2chandle.i2c_sda.i2c_gpio_port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PUPD_NONE, GPIO_OSPEED_50MHZ, i2chandle.i2c_sda.i2c_gpio_pin)
#define READ_SDA(i2chandle)   gpio_input_bit_get(i2chandle.i2c_sda.i2c_gpio_port,i2chandle.i2c_sda.i2c_gpio_pin)

#define IIC_SDA_H(i2chandle)  gpio_bit_set(i2chandle.i2c_sda.i2c_gpio_port,i2chandle.i2c_sda.i2c_gpio_pin)
#define IIC_SDA_L(i2chandle)  gpio_bit_reset(i2chandle.i2c_sda.i2c_gpio_port,i2chandle.i2c_sda.i2c_gpio_pin)

#define IIC_SCL_H(i2chandle)  gpio_bit_set(i2chandle.i2c_scl.i2c_gpio_port,i2chandle.i2c_scl.i2c_gpio_pin)
#define IIC_SCL_L(i2chandle)  gpio_bit_reset(i2chandle.i2c_scl.i2c_gpio_port,i2chandle.i2c_scl.i2c_gpio_pin)

#define IIC_NO_ACK (0)
#define IIC_ACK    (1)

void i2c_config(void);
void i2c_start(i2c_handle_t i2c);
void i2c_stop(i2c_handle_t i2c);
u8 i2c_wait_ack(i2c_handle_t i2c);
void i2c_ack(i2c_handle_t i2c);
void i2c_nack(i2c_handle_t i2c);

void i2c_write_byte(uint8_t txd,i2c_handle_t i2c);
u8 i2c_read_byte(uint8_t ack,i2c_handle_t i2c);

#endif
