/**************************************************************
 * 版权    : ZONKIN @2022
 * 文件名  : bsp_tmp100.c
 * 文件说明: tmp100配置文件
 * 创建人  : lht 2022-9-01
 * 修改时间: 
 * ***********************************************************/
#include "bsp_tmp100.h"
#include "bsp_i2c.h"


/**************************************************************
 * 函数名称: tmp100_init
 * 函数功能: tmp100 初始化
 * 输入参数: 
 * 输出参数: 无
 * 修改时间: 2022-9-01 / 9-05
 * ***********************************************************/
void tmp100_init(void)
{
    iic_init(I2C2);//总线初始化
    
    //5秒 等待(风扇控制板启动比较慢)
    for(uint8_t i=0;i<8;i++)
    {
        vTaskDelay(1000);
    }

    iic_config(I2C2, TMP100_IIC_ADDR_BIT7);
    
    tmp100_write(0x01,0x60);
}
/**************************************************************
 * 函数名称: tmp100_write
 * 函数功能: tmp100 写操作
 * 输入参数: uint8_t reg  : 寄存器
 *           uint8_t data : 数据
 * 输出参数: 无
 * 修改时间: 2022-9-01 / 9-05
 * ***********************************************************/
void tmp100_write(uint8_t reg, uint8_t data)
{
    char caller[13] = "tmp100_write";

    //等待 IIC 空闲
    iic_state_check(caller, caller, I2C2, I2C_FLAG_I2CBSY, 10);
    iic_start(caller, I2C2);
    iic_write_addr(caller, I2C2, TMP100_IIC_ADDR_BIT7, I2C_TRANSMITTER);
    iic_write_byte(caller, I2C2, reg);
    iic_write_byte(caller, I2C2, data);
    iic_stop(caller, I2C2);

    bmc_debug(1,"%s over\n",caller);
}
/**************************************************************
 * 函数名称: tmp100_read
 * 函数功能: tmp100 读操作
 * 输入参数: uint8_t reg : 寄存器
 * 输出参数: uint16_t    : 读出数据
 * 修改时间: 2022-9-01 / 9-05
 * ***********************************************************/
uint16_t tmp100_read(uint8_t reg)
{
    uint16_t ret = 0;
    char caller[12] = "tmp100_read";
    
    //等待 IIC 空闲
    iic_state_check(caller,caller,I2C2, I2C_FLAG_I2CBSY, 10);
    iic_start(caller, I2C2);
    iic_write_addr(caller, I2C2, TMP100_IIC_ADDR_BIT7, I2C_TRANSMITTER);//地址 + 写
    iic_write_byte(caller, I2C2, reg);//写 寄存器
    i2c_enable(I2C2);
    
    iic_start(caller, I2C2);
    iic_read_addr(caller, I2C2, TMP100_IIC_ADDR_BIT7, I2C_RECEIVER);//地址 + 读
    ret = (iic_read_byte(caller, I2C2, 1)<<8);//ACK
    ret |= iic_read_byte(caller, I2C2, 0);//NACK

    iic_stop(caller, I2C2);  //总线结束

    return ret;
}
/**************************************************************
 * 函数名称: get_tmp100_temperature
 * 函数功能: 获取tmp100温度值
 * 输入参数: 无
 * 输出参数: 无
 * 修改时间: 2022-9-01
 * ***********************************************************/
uint32_t get_tmp100_temperature(void)
{
    return (tmp100_read(0) >> 4)*0.0625;
}

