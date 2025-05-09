#include "i2c_ee.h"
#include "i2c_gpio.h"

// 检测EEPORM是否忙碌
uint8_t ee_Checkok(void)
{
    if (eeprom_i2c_CheckDevice(EEPROM_DEV_ADDR) == 0)
    {
        return 1;
    }
    else
    {
        eeprom_i2c_Stop();
        return 0;
    }
}

// 向EEPROM写入多个字节
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
    uint16_t i, m;
    uint16_t addr;
    addr = _usAddress;
    for (i = 0; i < _usSize; i++)
    {
        // 当第一次或者地址对齐到8就要重新发起起始信号和EEPROM地址
        // 为了解决8地址对齐问题
        if (i == 0 || (addr % EEPROM_PAGE_SIZE) == 0)
        {
            // 循环发送起始信号和EEPROM地址的原因是为了等待上一次写入的一页数据写入完成
            for (m = 0; m < 1000; m++)
            {
                // 发送起始地址
                eeprom_i2c_Start();
                // 发送设备写地址
                eeprom_i2c_SendByte(EEPROM_DEV_ADDR | EEPROM_I2C_WR);
                // 等待从机应答
                if (eeprom_i2c_WaitAck() == 0)
                {
                    break;
                }
            }
            // 若等待的1000次从机还未应答，等待超时
            if (m == 1000)
            {
                goto cmd_fail;
            }
            // EEPROM应答后发送EEPROM的内部存储器地址
            eeprom_i2c_SendByte((uint8_t)addr);
            // 等待从机应答
            if (eeprom_i2c_WaitAck() != 0)
            {
                goto cmd_fail;
            }
        }
        // 发送数据
        eeprom_i2c_SendByte(_pWriteBuf[i]);
        // 等待应答
        if (eeprom_i2c_WaitAck() != 0)
        {
            goto cmd_fail;
        }
        // 写入地址加1
        addr++;
    }

    eeprom_i2c_Stop();
    return 1;

cmd_fail:
    eeprom_i2c_Stop();
    return 0;
}
uint8_t eeprom_write(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
    uint8_t count = _usSize/16;
    uint8_t page_add = _usAddress % 16;
    int ret = 0;
    for (size_t a = 0; a < count; a++)
    {
        ret = ee_WriteBytes(_pWriteBuf+a*16, a*16+page_add, 16);
        delay_ms(5);
    }
    return ret;
}

uint8_t eeprom_read(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
    uint16_t i;

    eeprom_i2c_Start();
    eeprom_i2c_SendByte(EEPROM_DEV_ADDR | EEPROM_I2C_WR);
    if (eeprom_i2c_WaitAck() != 0)
    {
        goto cmd_fail;
    }
    eeprom_i2c_SendByte((uint8_t)_usAddress);

    if (eeprom_i2c_WaitAck() != 0)
    {
        goto cmd_fail;
    }
    eeprom_i2c_Start();
    eeprom_i2c_SendByte(EEPROM_DEV_ADDR | EEPROM_I2C_RD);
    if (eeprom_i2c_WaitAck() != 0)
    {
        goto cmd_fail;
    }
    for (i = 0; i < _usSize; i++)
    {
        _pReadBuf[i] = eeprom_i2c_ReadByte();
        // printf("11111:%2X\r\n", _pReadBuf[i]);
        /* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
        if (i != _usSize - 1)
        {
            //			eeprom_i2c_NAcK();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
            eeprom_i2c_Ack(); /* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
        }
        else
        {
            eeprom_i2c_NAcK(); /* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
        }
    }
    eeprom_i2c_Stop();
    return 1;

cmd_fail:
    eeprom_i2c_Stop();
    return 0;
}

uint8_t ee_Test(void)
{
    uint16_t i;
    uint8_t write_buf[EEPROM_SIZE] = {0};
    uint8_t read_buf[EEPROM_SIZE] = {0};

    /*-----------------------------------------------------------------------------------*/
    if (eeprom_i2c_CheckDevice(EEPROM_DEV_ADDR) == 1)
    {
        /* 没有检测到EEPROM */
        printf("IIC EEPROM ERROR!\r\n");
        return 0;
    }
    /*------------------------------------------------------------------------------------*/
    /* 填充测试缓冲区 */
    for (i = 0; i < EEPROM_SIZE; i++)
    {
        write_buf[i] = i;
    }
    /*------------------------------------------------------------------------------------*/
    if (eeprom_write(write_buf, 0, EEPROM_SIZE) == 0)
    {
        printf("WRITE EEPROM ERROR!\r\n");
        return 0;
    }
    else
    {
        printf("WRITE EEPROM SUCCESS!\r\n");
    }
    delay_ms(10);
    /*-----------------------------------------------------------------------------------*/
    if (eeprom_read(read_buf, 0, EEPROM_SIZE) == 0)
    {
        printf("READ EEPROM ERROR!\r\n");
        return 0;
    }
    else
    {
        printf("READ EEPROM SUCCESS, Data:\r\n");
    }
    /*-----------------------------------------------------------------------------------*/
    for (i = 0; i < EEPROM_SIZE; i++)
    {
        if (read_buf[i] != write_buf[i])
        {
            printf("write:0x%02X ---", write_buf[i]);
            printf("read:0x%02X \r\n", read_buf[i]);
            printf("ERROR:EEPROM data inconsistent\r\n");
            return 0;
        }
        printf(" %02X", read_buf[i]);

        if ((i & 15) == 15)
        {
            printf("\r\n");
        }
    }
    printf("EEPROM  WRITE READ SUCCESS\r\n");
    return 1;
}

SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    ee_Test, ee_Test, eeP_test vaule);


// int ee_write(int argc, char *argv[])
// {
//     uint8_t write_buf[256] = {0};
//     uint8_t read_buf[256] = {0};
    
//         for (size_t i = 0; i < 256; i++)
//         {
//             write_buf[i] = i;
//         }
//         eeprom_write(write_buf, 0, 256);
//         eeprom_read(read_buf, 0, 256);
//         for (size_t i = 0; i < 256; i++)
//         {
//             printf("data-%d: write:%X --- read:%X\r\n", i, write_buf[i], read_buf[i]);
//         }
// }

// SHELL_EXPORT_CMD(
//     SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
//     write, ee_write, eeP_test vaule);
