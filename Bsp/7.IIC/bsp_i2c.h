#ifndef _BSP_I2C_H_
#define _BSP_I2C_H_

#include "main.h"

#define IIC_CLOCK_SPEED(x) (x * 1000)  
#define IIC_DEFAULT_ADDR   (0x32 << 1)
#define IIC_STOP_MASK      (0x0200)
#define IIC_ERR_OFFSET     (0x10)
//#define IIC_CTL0_STOP      (0x0B)

typedef enum _iic_workmode{
    iic_SendMode = 0,  //发送模式
    iic_RecvMode       //接收模式
}iic_workmode;

typedef enum _iic_state_flag{
    IIC_FLAG_OK = 0,
    IIC_FLAG_SBSEND_ERR   = 0x10,
    IIC_FLAG_ADDSEND_ERR  = 0x11,
    IIC_FLAG_BTC_ERR      = 0x12,
    IIC_FLAG_RBNE_ERR     = 0x18,
    IIC_FLAG_TBE_ERR      = 0x19,
    IIC_FLAG_BUSY_ERR     = 0x21,
    IIC_FLAG_CTL0STOP_ERR = 0x22,
    IIC_FLAG_TIMEOUT_ERR  = 0x30,
    IIC_FLAG_WITHOUT_ERR  = 0x31
}iic_state;

void masterSend(void);
void masterRec(void);
void iic_init(uint32_t i2c_periph);
void iic_config(uint32_t i2c_periph, uint8_t i2c_addr);
void iic_nvic_config(uint32_t i2c_periph);
void iic_mode_switch(uint32_t i2c_periph, iic_workmode mode);
iic_state iic_state_check(char *caller1, char *caller2, uint32_t i2c_periph, uint16_t flag, uint16_t timeout);
iic_state iic_start(char *caller, uint32_t i2c_periph);
iic_state iic_stop(char *caller, uint32_t i2c_periph);
iic_state iic_write_addr(char *caller, uint32_t i2c_periph, uint8_t addr, uint32_t trandirection);
iic_state iic_read_addr(char *caller, uint32_t i2c_periph, uint8_t addr, uint32_t trandirection);
iic_state iic_write_byte(char *caller, uint32_t i2c_periph, uint8_t byte);
uint8_t iic_read_byte(char *caller, uint32_t i2c_periph, uint8_t ack);
void iic_event_irq_handle(uint32_t i2c_periph);
void iic_error_irq_handle(uint32_t i2c_periph);

#endif /* _BSP_I2C_H_ */
