/**************************************************************
 * 版权    : ZONKIN @2022
 * 文件名  : bsp_nct210.c
 * 文件说明: ntc210配置文件
 * 创建人  : lht 2022-9-01
 * 修改时间: 
 * ***********************************************************/
#include "bsp_nct210.h"
#include "bsp_i2c.h"


/**************************************************************
 * 函数名称: nct210_init
 * 函数功能: nct210 初始化
 * 输入参数: 
 * 输出参数: 无
 * 修改时间: 2022-9-01 / 9-05
 * ***********************************************************/
void nct210_init(void)
{
    iic_config(I2C2, NCT210_IIC_ADDR_BIT7);
    
    nct210_write(0x09,0x80);
    
    nct210_write(0x0A,0x00);
}

/**************************************************************
 * 函数名称: nct210_write
 * 函数功能: nct210 写操作
 * 输入参数: uint8_t reg,uint8_t data
 * 输出参数: 无
 * 修改时间: 2022-9-01 / 9-05
 * ***********************************************************/
void nct210_write(uint8_t reg,uint8_t data)
{
    char caller[13] = "nct210_write";

    //等待 IIC 空闲
    iic_state_check(caller, caller, I2C2, I2C_FLAG_I2CBSY, 10);
    iic_start(caller, I2C2);
    iic_write_addr(caller, I2C2, NCT210_IIC_ADDR_BIT7, I2C_TRANSMITTER);
    iic_write_byte(caller, I2C2, reg);
    iic_write_byte(caller, I2C2, data);
    iic_stop(caller, I2C2);
    
    bmc_debug(1,"%s over\n",caller);
}
/**************************************************************
 * 函数名称: nct210_read
 * 函数功能: nct210 读操作
 * 输入参数: uint8_t reg,uint8_t data
 * 输出参数: uint8_t
 * 修改时间: 2022-9-01 / 9-05
 * ***********************************************************/
uint8_t nct210_read(uint8_t reg)
{
    uint8_t ret = 0;
    char caller[12] = "nct210_read";
    
    //等待 IIC 空闲
    iic_state_check(caller, caller, I2C2, I2C_FLAG_I2CBSY, 10);
    iic_start(caller, I2C2);
    iic_write_addr(caller, I2C2, NCT210_IIC_ADDR_BIT7, I2C_TRANSMITTER);//地址 + 写
    iic_write_byte(caller, I2C2, reg);//写 寄存器
    iic_stop(caller, I2C2);  //总线结束
    iic_state_check(caller, caller, I2C2, I2C_FLAG_I2CBSY, 10);
    iic_start(caller, I2C2);
    iic_read_addr(caller, I2C2, NCT210_IIC_ADDR_BIT7, I2C_RECEIVER);//地址 + 读

    ret = iic_read_byte(caller, I2C2, 0); //NACK

    iic_stop(caller, I2C2);  //总线结束
    
    return ret;
}
/**************************************************************
 * 函数名称: get_nct210_temperature
 * 函数功能: nct210 获取温度
 * 输入参数: 
 * 输出参数: 
 * 修改时间: 2022-9-01
 * ***********************************************************/
uint8_t get_nct210_temperature(void)
{
    return nct210_read(0);
}

