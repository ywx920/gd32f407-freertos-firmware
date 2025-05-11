#ifndef _BSP_I2C_H_
#define _BSP_I2C_H_

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

#define IIC_CLOCK_SPEED(x) (x * 1000)
#define IIC_DEFAULT_ADDR (0x32 << 1)
#define IIC_STOP_MASK (0x0200)
#define IIC_ERR_OFFSET (0x10)
#define IIC_CTL0_STOP (0x0B)

/* STM32 I2C 快速模式 */
#define I2C_Speed 100000 //*

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2Cx_OWN_ADDRESS7 0X82

/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))
static __IO uint32_t I2CTimeout = I2CT_LONG_TIMEOUT;

/*信息输出*/
#define I2C_DEBUG_ON 0

#define I2C_INFO(fmt, arg...) printf("<<-I2C-INFO->> " fmt "\n", ##arg)
#define I2C_ERROR(fmt, arg...) printf("<<-I2C-ERROR->> " fmt "\n", ##arg)
#define I2C_DEBUG(fmt, arg...)                                        \
    do                                                                \
    {                                                                 \
        if (I2C_DEBUG_ON)                                             \
            printf("<<-I2C-DEBUG->> [%d]" fmt "\n", __LINE__, ##arg); \
    } while (0)

#define I2C_SLAVE_ADDRESS 0xA0

uint32_t I2C_SLAVE_ByteWrite(uint32_t i2cx, uint8_t device_addr, const uint8_t *p_buffer, uint16_t len);

void I2C_SLAVE_WaitEepromStandbyState(void);
static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

typedef enum _iic_workmode
{
    iic_SendMode = 0, // 发送模式
    iic_RecvMode      // 接收模式
} iic_workmode;

typedef enum _iic_state_flag
{
    IIC_FLAG_OK = 0,
    IIC_FLAG_SBSEND_ERR = 0x10,
    IIC_FLAG_ADDSEND_ERR = 0x11,
    IIC_FLAG_BTC_ERR = 0x12,
    IIC_FLAG_RBNE_ERR = 0x18,
    IIC_FLAG_TBE_ERR = 0x19,
    IIC_FLAG_BUSY_ERR = 0x21,
    IIC_FLAG_CTL0STOP_ERR = 0x22,
    IIC_FLAG_TIMEOUT_ERR = 0x30,
    IIC_FLAG_WITHOUT_ERR = 0x31
} iic_state;

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

void I2C_Send_Data(unsigned char slaveadr, unsigned char data);

uint32_t i2c0_bytes_read(uint32_t i2cx, uint8_t device_addr, uint8_t read_addr, uint8_t *p_buffer, uint16_t len);
uint32_t i2c0_bytes_write(uint32_t i2cx, uint8_t device_addr, const uint8_t *p_buffer, uint16_t len);
uint32_t i2c0_master(uint32_t i2cx, uint8_t device_addr, uint8_t read_addr, uint8_t *p_buffer, uint16_t len); //, uint16_t len

#define I2C0_SLAVE_ADDRESS7 0x82
#define I2C1_SLAVE_ADDRESS7 0x72

typedef enum
{
    open_iic0 = 0x01,
    open_iic1 = 0x02
} iic_select;

void iic_Init(iic_select select, ...);

/* iic0的中断回调函数 */
void I2C0_EventIRQ_Handler(void);
/* iic1的中断回调函数 */
// void I2C1_EventIRQ_Handler(void);

void I2C0_ErrorIRQ_Handler(void);
// void I2C1_ErrorIRQ_Handler(void);

extern volatile ErrStatus status;
// extern volatile uint8_t   *i2c_txbuffer;
extern volatile uint8_t *i2c_rxbuffer;
extern volatile uint16_t i2c_nbytes;

/* function declarations */
/* handle I2C0 event interrupt request */
void i2c0_event_irq_handler(void);
/* handle I2C0 error interrupt request */
void i2c0_error_irq_handler(void);
void i2c_nvic_config(void);

void i2c0_irq_test(void);

#endif /* _BSP_I2C_H_ */
