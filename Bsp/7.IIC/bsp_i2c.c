/**************************************************************
 * 版权    : ZONKIN @2022
 * 文件名  : bsp_i2c.c
 * 文件说明: i2c配置文件
 * 创建人  : lht 2022-9-01
 * 修改时间:
 * ***********************************************************/

#include "bsp_i2c.h"

uint8_t gi2c_recv = 0;
#define I2C0_OWN_ADDRESS7 0x82
#define I2C0_SPEED 100000
#define I2C0_SLAVE_ADDRESS7 0xA0
/*
    PB5---I2C0_SMBA   CPU_I2C0_ALERT_N   SMB_ALERT
    PB6---I2C0_SCL    CPU_I2C0_SCL       SMB_CK
    PB7---I2C0_SDA    CPU_I2C0_SDA       SMB_DAT

    PF0---I2C1_SDA    CPU_I2C1_SDA       I2C_DAT
    PF1---I2C1_SCL    CPU_I2C1_SCL       I2C_CK

    PC9---I2C2_SDA
    PA8---I2C2_SCL
*/
//

/**************************************************************
 * 函数名称: iic_init
 * 函数功能: iic初始化
 * 输入参数: i2c_periph: I2C0~2
 * 输出参数: 无
 * 修改时间: 2022-9-01
 * ***********************************************************/
void iic_init(uint32_t i2c_periph)
{
    switch (i2c_periph)
    {
    case I2C0:

        /* enable GPIOB clock */
        rcu_periph_clock_enable(RCU_GPIOB);
        /* enable I2C0 clock */
        rcu_periph_clock_enable(RCU_I2C0);
        /* I2C0 GPIO ports */
        /* connect PB6 to I2C0_SCL */
        gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_6);
        /* connect PB7 to I2C0_SDA */
        gpio_af_set(GPIOB, GPIO_AF_4, GPIO_PIN_7);

        /* configure I2C0 GPIO */
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

        /* configure I2C clock */
        i2c_clock_config(I2C0, I2C0_SPEED, I2C_DTCY_2);
        /* configure I2C address */
        i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_SLAVE_ADDRESS7);
        /* enable I2C0 */
        i2c_enable(I2C0);
        /* enable acknowledge */
        i2c_ack_config(I2C0, I2C_ACK_ENABLE);

        break;
    case I2C1:
        /* 总线时钟 */
        // rcu_periph_clock_enable(RCU_GPIOF);
        /* IIC1 时钟 打开 */
        // rcu_periph_clock_enable(RCU_I2C1);
        /*
         * PF0 I2C1_SDA
         * PF1 I2C1_SCL
         */
        // gpio_init(GPIOF, GPIO_MODE_AF, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1); // GPIO_OTYPE_OD,

        /* enable GPIOF clock */
        rcu_periph_clock_enable(RCU_GPIOF);
        /* enable I2C1 clock */
        rcu_periph_clock_enable(RCU_I2C1);
        /* connect PF1 to I2C1_SDA */
        gpio_af_set(GPIOF, GPIO_AF_4, GPIO_PIN_1);
        /* connect PF0 to I2C1_SDA */
        gpio_af_set(GPIOF, GPIO_AF_4, GPIO_PIN_0);

        /* configure I2C0 GPIO */
        gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_1);
        gpio_output_options_set(GPIOF, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
        gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0);
        gpio_output_options_set(GPIOF, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_0);

        /* configure I2C clock */
        // i2c_clock_config(I2C1, 100000, I2C_DTCY_2);
        // /* configure I2C address */
        // i2c_mode_addr_config(I2C1, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_OWN_ADDRESS7); // 地址待改
        // /* enable I2C0 */
        // i2c_enable(I2C1);
        // /* enable acknowledge */
        // i2c_ack_config(I2C1, I2C_ACK_ENABLE);

        break;
    case I2C2:
        /* 总线时钟 */
        // rcu_periph_clock_enable(RCU_GPIOA);
        // rcu_periph_clock_enable(RCU_GPIOC);
        // /* enable I2C2 clock */
        // rcu_periph_clock_enable(RCU_I2C2);
        // /* 备用时钟 */
        // rcu_periph_clock_enable(RCU_AF);
        // /* IIC 映射脚 使能 */
        // gpio_pin_remap1_config(GPIO_PCF5,GPIO_PCF5_I2C2_REMAP0,ENABLE);
        // /* 根据参考手册获悉 映射到 I2C2_REMAP0 */
        // gpio_init(GPIOA, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
        // gpio_init(GPIOC, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
        break;
    default:
        bmc_debug(1, "%s i2c_periph = %d is error\n", __func__, i2c_periph);
        break;
    }
}

/************************ TEST I2C ************************/

/*
    1、主从设备建立I2C通信
    2、主设备发送从设备标识符地址
    3、从设备接收并返回对应地址的数据
    4、主设备接收数据并处理
     */

// uint32_t i2cxWriteAddr()
// {

// }

// uint32_t i2cxReadAddr()
// {

// }

// uint32_t i2cxWriteByte()
// {
//     int i = 0;

//     /* wait until I2C bus is idle  */
//     while (i2c_flag_get(i2cx, I2C_FLAG_I2CBSY))
//     {
//         if ((I2CTimeout--) == 0)
//             return I2C_TIMEOUT_UserCallback(0);
//     }

//     /* send a start condition to I2C bus */
//     i2c_start_on_bus(i2cx);
//     /* wait until SBSEND bit is set */
//     while (!i2c_flag_get(i2cx, I2C_FLAG_SBSEND))
//     {
//         if ((I2CTimeout--) == 0)
//             return I2C_TIMEOUT_UserCallback(1);
//     }

//     /* send slave address to I2C bus */
//     i2c_master_addressing(i2cx, device_addr, I2C_TRANSMITTER);
//     /* wait until ADDSEND bit is set */
//     while (!i2c_flag_get(i2cx, I2C_FLAG_ADDSEND))
//     {
//         if ((I2CTimeout--) == 0)
//             return I2C_TIMEOUT_UserCallback(2);
//     }

//     /* clear ADDSEND bit */
//     i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);
//     /* wait until the transmit data buffer is empty */
//     while (!i2c_flag_get(i2cx, I2C_FLAG_TBE))
//     {
//         if ((I2CTimeout--) == 0)
//             return I2C_TIMEOUT_UserCallback(3);
//     }

//     for (i = 0; i < len; i++)
//     {
//         /* data transmission */
//         i2c_data_transmit(i2cx, p_buffer[i]);
//         /* wait until the TBE bit is set */
//         while (!i2c_flag_get(i2cx, I2C_FLAG_TBE))
//         {
//             if ((I2CTimeout--) == 0)
//                 return I2C_TIMEOUT_UserCallback(4);
//         }
//     }

//     /* send a stop condition to I2C bus */
//     i2c_stop_on_bus(i2cx);
//     while (I2C_CTL0(i2cx) & 0x0200)
//     {
//         if ((I2CTimeout--) == 0)
//             return I2C_TIMEOUT_UserCallback(5);
//     }

//     return 1;
// }

// uint32_t i2cxReadByte()
// {

// }

uint32_t i2c0_master(uint32_t i2cx, uint8_t device_addr, uint8_t read_addr, uint8_t *p_buffer, uint16_t len) //, uint16_t len
{
    // uint8_t rx_buffer2[10];

    I2CTimeout = I2CT_LONG_TIMEOUT;
    /* wait until I2C bus is idle */
    while (i2c_flag_get(i2cx, I2C_FLAG_I2CBSY))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(6);
    }

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2cx);
    /* wait until SBSEND bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_SBSEND))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(7);
    }

    /* send slave address to I2C bus */
    i2c_master_addressing(i2cx, device_addr, I2C_TRANSMITTER);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* wait until ADDSEND bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_ADDSEND))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(8);
    }

    /* clear ADDSEND bit */
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);
    /* wait until the transmit data buffer is empty */
    while (!i2c_flag_get(i2cx, I2C_FLAG_TBE))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(9);
    }

    /* send the internal address to write to */
    i2c_data_transmit(i2cx, read_addr);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* wait until the BTC bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_BTC))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(10);
    }

    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2cx);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* wait until SBSEND bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_SBSEND))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(11);
    }

    /* send slave address to I2C bus */
    i2c_master_addressing(i2cx, device_addr, I2C_RECEIVER);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* wait until ADDSEND bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_ADDSEND))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(12);
    }
    /* clear the ADDSEND bit */
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    while (len)
    {
        // 修改后的关键逻辑片段
        if (len > 1)
        {
            i2c_ackpos_config(i2cx, I2C_ACKPOS_NEXT);
        }
        else
        {
            i2c_ack_config(i2cx, I2C_ACK_DISABLE);
            i2c_stop_on_bus(i2cx); // 提前发送STOP
        }

        I2CTimeout = I2CT_LONG_TIMEOUT;
        // for (int i = 0; i < len; i++)
        // {
        while (!i2c_flag_get(i2cx, I2C_FLAG_RBNE))
        {
            if ((I2CTimeout--) == 0)
                return I2C_TIMEOUT_UserCallback(13);
        }; // 严格等待每个字节
        {
            *p_buffer = i2c_data_receive(i2cx);
            // delay_ms(100);
            
            // p_buffer++;
            // len--;
        }
        // if (i == len - 2)
        // { // 倒数第二个字节
        //     i2c_ack_config(i2cx, I2C_ACK_DISABLE);
        //     i2c_stop_on_bus(i2cx);
        // }
        // }
        // delay_1ms(100);
    }

    // i2c_ackpos_config(i2cx, I2C_ACKPOS_NEXT); // 这一步是关键
    // while (len)
    // {
    //     if (1 == len)
    //     {
    //         i2c_ackpos_config(i2cx, I2C_ACKPOS_CURRENT); // 这一步是关键
    //         i2c_ack_config(i2cx, I2C_ACK_DISABLE);
    //     }
    //     if (i2c_flag_get(i2cx, I2C_FLAG_RBNE))
    //     {
    //         *p_buffer = i2c_data_receive(i2cx);
    //         i2c_ack_config(i2cx, I2C_ACK_ENABLE);
    //         p_buffer++;
    //         len--;
    //     }
    //     i2c_flag_clear(i2cx, I2C_FLAG_RBNE);
    // }

    // for (uint8_t i = 0; i < 10; i++)
    // {
    //     if (i == 9)
    //     {                                          // 倒数第二个字节后配置NACK
    //         i2c_ack_config(I2C0, I2C_ACK_DISABLE); // 禁用ACK
    //         i2c_stop_on_bus(I2C0);                 // 提前发送停止条件
    //     }
    //     while (!i2c_flag_get(I2C0, I2C_FLAG_RBNE))
    //     {
    //         if ((I2CTimeout--) == 0)
    //             return I2C_TIMEOUT_UserCallback(13);
    //     } // 等待接收缓冲区非空
    //     p_buffer[i] = i2c_data_receive(I2C0); // 读取数据到数组
    // }

    // while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY))
    // {
    //     if ((I2CTimeout--) == 0)
    //         return I2C_TIMEOUT_UserCallback(14);
    // } // 等待总线空闲
    // i2c_ack_config(I2C0, I2C_ACK_ENABLE); // 恢复ACK状态

    // if (1 == len)
    // {
    //     /* send a stop condition to I2C bus */
    //     i2c_stop_on_bus(i2cx);
    // }

    /* while there is data to be read */
    // while (len)
    // {
    //     if (3 == len)
    //     {
    //         /* wait until the BTC bit is set */
    //         while (!i2c_flag_get(i2cx, I2C_FLAG_BTC))
    //         {
    //             if ((I2CTimeout--) == 0)
    //                 return I2C_TIMEOUT_UserCallback(13);
    //         }
    //         /* disable acknowledge */
    //         i2c_ack_config(i2cx, I2C_ACK_DISABLE);
    //     }
    //     else if (2 == len)
    //     {
    //         /* wait until the BTC bit is set */
    //         while (!i2c_flag_get(i2cx, I2C_FLAG_BTC))
    //         {
    //             if ((I2CTimeout--) == 0)
    //                 return I2C_TIMEOUT_UserCallback(14);
    //         }
    //         /* send a stop condition to I2C bus */
    //         i2c_stop_on_bus(i2cx);
    //     }

    //     for (uint16_t i = 0; i < len;)
    //     {
    //         if (!i2c_flag_get(i2cx, I2C_FLAG_RBNE))
    //         {
    //             p_buffer[i++] = i2c_data_receive(i2cx);
    //             // i2c_ackpos_config(i2cx, I2C_ACKPOS_NEXT);
    //         }
    //     }

    /* wait until the RBNE bit is set and clear it */
    // while (!i2c_flag_get(i2cx, I2C_FLAG_RBNE))
    // {
    //     /* read a byte from the EEPROM */
    //     // teatData = i2c_data_receive(i2cx); // 逐字节存入数组
    //     // *p_buffer = i2c_data_receive(i2cx);
    //     /* point to the next location where the byte read will be saved */
    //     // p_buffer++;
    //     /* decrement the read bytes counter */
    //     // len--;
    // }
    // }

    /* wait until the stop condition is finished */
    // while (I2C_CTL0(i2cx) & 0x0200)
    // {
    //     if ((I2CTimeout--) == 0)
    //         return I2C_TIMEOUT_UserCallback(15);
    // }
    /* enable acknowledge */
    i2c_ack_config(i2cx, I2C_ACK_ENABLE);
    i2c_ackpos_config(i2cx, I2C_ACKPOS_CURRENT);

    return 1;
}

uint32_t i2c0_bytes_write(uint32_t i2cx, uint8_t device_addr, const uint8_t *p_buffer, uint16_t len)
{
    int i = 0;
    // iic_init(I2C0);

    /* 等待总线空闲 wait until I2C bus is idle  */
    while (i2c_flag_get(i2cx, I2C_FLAG_I2CBSY))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }

    /* 1 发送开始条件 send a start condition to I2C bus */
    i2c_start_on_bus(i2cx);
    /* 判断开始是否成功 wait until SBSEND bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_SBSEND))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(1);
    }

    /* 2 发送从设备地址 send slave address to I2C bus */
    i2c_master_addressing(i2cx, device_addr, I2C_TRANSMITTER);
    /* 判断发送是否成功 wait until ADDSEND bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_ADDSEND))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(2);
    }

    /* 3 清除发送标志 clear ADDSEND bit */
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);
    /* 等待发送缓冲区为空 wait until the transmit data buffer is empty */
    while (!i2c_flag_get(i2cx, I2C_FLAG_TBE))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(3);
    }

    for (i = 0; i < len; i++)
    {
        /* 数据发送 data transmission */
        i2c_data_transmit(i2cx, p_buffer[i]);
        /* 判断发送成功标志 wait until the TBE bit is set */
        while (!i2c_flag_get(i2cx, I2C_FLAG_TBE))
        {
            if ((I2CTimeout--) == 0)
                return I2C_TIMEOUT_UserCallback(4);
        }
    }

    /* 发送停止条件 send a stop condition to I2C bus */
    i2c_stop_on_bus(i2cx);
    while (I2C_CTL0(i2cx) & 0x0200)
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(5);
    }

    return 1;
}

uint32_t i2c0_bytes_read(uint32_t i2cx, uint8_t device_addr, uint8_t read_addr, uint8_t *p_buffer, uint16_t len)
{
    //   i2c0_start();
    // iic_init(I2C0);

    /* wait until I2C bus is idle */
    while (i2c_flag_get(i2cx, I2C_FLAG_I2CBSY))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(6);
    }

    /* 当需要读取两个字节的数据时，确保在接收到第一个字节后，继续读取第二个字节
        并在第二个字节数据接收完成后发送一个NACK，表示数据接收完成 */
    if (2 == len)
    {
        i2c_ackpos_config(i2cx, I2C_ACKPOS_NEXT);
    }

    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2cx);
    /* wait until SBSEND bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_SBSEND))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(7);
    }

    /* send slave address to I2C bus */
    i2c_master_addressing(i2cx, device_addr, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_ADDSEND))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(8);
    }

    /* clear ADDSEND bit */
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);
    /* wait until the transmit data buffer is empty */
    while (!i2c_flag_get(i2cx, I2C_FLAG_TBE))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(9);
    }

    /* enable I2C0*/
    i2c_enable(i2cx);

    /* send the internal address to write to */
    i2c_data_transmit(i2cx, read_addr);
    /* wait until the BTC bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_BTC))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(10);
    }

    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2cx);
    /* wait until SBSEND bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_SBSEND))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(11);
    }

    /* send slave address to I2C bus */
    i2c_master_addressing(i2cx, device_addr, I2C_RECEIVER);

    if (len < 3)
    {
        /* disable acknowledge */
        i2c_ack_config(i2cx, I2C_ACK_DISABLE);
    }

    /* wait until ADDSEND bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_ADDSEND))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(12);
    }
    /* clear the ADDSEND bit */
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    if (1 == len)
    {
        /* send a stop condition to I2C bus */
        i2c_stop_on_bus(i2cx);
    }

    /* while there is data to be read */
    while (len)
    {
        if (3 == len)
        {
            /* wait until the BTC bit is set */
            while (!i2c_flag_get(i2cx, I2C_FLAG_BTC))
            {
                if ((I2CTimeout--) == 0)
                    return I2C_TIMEOUT_UserCallback(13);
            }
            /* disable acknowledge */
            i2c_ack_config(i2cx, I2C_ACK_DISABLE);
        }
        else if (2 == len)
        {
            /* wait until the BTC bit is set */
            while (!i2c_flag_get(i2cx, I2C_FLAG_BTC))
            {
                if ((I2CTimeout--) == 0)
                    return I2C_TIMEOUT_UserCallback(14);
            }
            /* send a stop condition to I2C bus */
            i2c_stop_on_bus(i2cx);
        }

        /* wait until the RBNE bit is set and clear it */
        if (i2c_flag_get(i2cx, I2C_FLAG_RBNE))
        {
            /* read a byte from the EEPROM */
            *p_buffer = i2c_data_receive(i2cx);
            /* point to the next location where the byte read will be saved */
            p_buffer++;
            /* decrement the read bytes counter */
            len--;
        }
    }

    /* wait until the stop condition is finished */
    while (I2C_CTL0(i2cx) & 0x0200)
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(15);
    }
    /* enable acknowledge */
    i2c_ack_config(i2cx, I2C_ACK_ENABLE);
    i2c_ackpos_config(i2cx, I2C_ACKPOS_CURRENT);

    return 1;
}

uint32_t event1;

/*!
    \brief      handle I2C0 event interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c0_event_irq_handler(void)
{
    if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_SBSEND))
    {
        /* the master sends slave address */
        i2c_master_addressing(I2C0, I2C1_SLAVE_ADDRESS7, I2C_RECEIVER);
    }
    else if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_ADDSEND))
    {
        if ((1 == i2c_nbytes) || (2 == i2c_nbytes))
        {
            /* clear the ACKEN before the ADDSEND is cleared */
            i2c_ack_config(I2C0, I2C_ACK_DISABLE);
            /* clear the ADDSEND bit */
            i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_ADDSEND);
        }
        else
        {
            /* clear the ADDSEND bit */
            i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_ADDSEND);
        }
    }
    else if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_RBNE))
    {
        if (i2c_nbytes > 0)
        {
            if (3 == i2c_nbytes)
            {
                /* wait until the second last data byte is received into the shift register */
                while (!i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_BTC))
                    ;
                /* send a NACK for the last data byte */
                i2c_ack_config(I2C0, I2C_ACK_DISABLE);
            }
            /* read a data byte from I2C_DATA*/
            *i2c_rxbuffer++ = i2c_data_receive(I2C0);
            i2c_nbytes--;
            if (0 == i2c_nbytes)
            {
                /* send a stop condition */
                i2c_stop_on_bus(I2C0);
                status = SUCCESS;
                i2c_ack_config(I2C0, I2C_ACK_ENABLE);
                i2c_ackpos_config(I2C0, I2C_ACKPOS_CURRENT);
                /* disable the I2C0 interrupt */
                i2c_interrupt_disable(I2C0, I2C_INT_ERR);
                i2c_interrupt_disable(I2C0, I2C_INT_BUF);
                i2c_interrupt_disable(I2C0, I2C_INT_EV);
            }
        }
    }
}

/*!
    \brief      handle I2C0 error interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c0_error_irq_handler(void)
{
    /* no acknowledge received */
    if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_AERR))
    {
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_AERR);
    }

    /* SMBus alert */
    if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_SMBALT))
    {
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_SMBALT);
    }

    /* bus timeout in SMBus mode */
    if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_SMBTO))
    {
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_SMBTO);
    }

    /* over-run or under-run when SCL stretch is disabled */
    if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_OUERR))
    {
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_OUERR);
    }

    /* arbitration lost */
    if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_LOSTARB))
    {
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_LOSTARB);
    }

    /* bus error */
    if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_BERR))
    {
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_BERR);
    }

    /* CRC value doesn't match */
    if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_PECERR))
    {
        i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_PECERR);
    }

    /* disable the I2C0 interrupt */
    i2c_interrupt_disable(I2C0, I2C_INT_ERR);
    i2c_interrupt_disable(I2C0, I2C_INT_BUF);
    i2c_interrupt_disable(I2C0, I2C_INT_EV);
}

void i2c_nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(I2C0_EV_IRQn, 0, 3);
    // nvic_irq_enable(I2C1_EV_IRQn, 0, 4);
    nvic_irq_enable(I2C0_ER_IRQn, 0, 2);
    // nvic_irq_enable(I2C1_ER_IRQn, 0, 1);
}

/* I2C 中断接收测试 */
uint8_t i2c_buffer_receiver[16];
volatile uint8_t *i2c_rxbuffer;
volatile uint16_t i2c_nbytes;
volatile ErrStatus status;
ErrStatus state = ERROR;

void i2c0_irq_test(void)
{

    printf(" iic irq test \r\n");

    i2c_rxbuffer = i2c_buffer_receiver;
    i2c_nbytes = 16;
    status = ERROR;

    /* enable the I2C0 interrupt */
    i2c_interrupt_enable(I2C0, I2C_INT_ERR);
    i2c_interrupt_enable(I2C0, I2C_INT_EV);
    i2c_interrupt_enable(I2C0, I2C_INT_BUF);

    if (2 == i2c_nbytes)
    {
        /* send ACK for the next byte */
        i2c_ackpos_config(I2C0, I2C_ACKPOS_NEXT);
    }
    /* the master waits until the I2C bus is idle */
    while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY))
        ;
    /* the master sends a start condition to I2C bus */
    i2c_start_on_bus(I2C0);

    while (i2c_nbytes > 0)
        ;
    while (SUCCESS != status)
        ;

    // 打印接收到的数据
    if (SUCCESS == status)
    {
        printf("Received data from I2C0:\r\n");
        for (int i = 0; i < i2c_nbytes; i++)
        {
            printf("%02X ", i2c_buffer_receiver[i]);
        }
        printf("\r\n");
    }
    else
    {
        printf("I2C communication failed!\r\n");
    }
}

uint32_t I2C_SLAVE_ByteWrite(uint32_t i2cx, uint8_t device_addr, const uint8_t *p_buffer, uint16_t len)
{

    int i = 0;
    // iic_init(I2C0);

    /* 等待总线空闲 wait until I2C bus is idle  */
    while (i2c_flag_get(i2cx, I2C_FLAG_I2CBSY))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }

    /* 1 发送开始条件 send a start condition to I2C bus */
    i2c_start_on_bus(i2cx);
    /* 判断开始是否成功 wait until SBSEND bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_SBSEND))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(1);
    }

    /* 2 发送从设备地址 send slave address to I2C bus */
    i2c_master_addressing(i2cx, device_addr, I2C_TRANSMITTER);
    /* 判断发送是否成功 wait until ADDSEND bit is set */
    while (!i2c_flag_get(i2cx, I2C_FLAG_ADDSEND))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(2);
    }

    /* 3 清除发送标志 clear ADDSEND bit */
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);
    /* 等待发送缓冲区为空 wait until the transmit data buffer is empty */
    while (!i2c_flag_get(i2cx, I2C_FLAG_TBE))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(3);
    }

    for (i = 0; i < len; i++)
    {
        /* 数据发送 data transmission */
        i2c_data_transmit(i2cx, p_buffer[i]);
        /* 判断发送成功标志 wait until the TBE bit is set */
        while (!i2c_flag_get(i2cx, I2C_FLAG_TBE))
        {
            if ((I2CTimeout--) == 0)
                return I2C_TIMEOUT_UserCallback(4);
        }
    }
    /* 发送停止条件 send a stop condition to I2C bus */
    i2c_stop_on_bus(i2cx);
    while (I2C_CTL0(i2cx) & 0x0200)
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(5);
    }
}

/**
 * @brief  Wait for SLAVE DEV Standby state
 * @param  无
 * @retval 无
 */
// void I2C_SLAVE_WaitEepromStandbyState(void)
// {
//     uint16_t SR1_Tmp = 0;

//     do
//     {
//         /* Send START condition */
//         i2c_start_on_bus(i2c0);

//         /* 读寄存器 Read I2C1 SR1 register */

//         SR1_Tmp = I2C_ReadRegister(EEPROM_I2Cx, I2C_Register_SR1);
//         /* 发送待写入的地址 Send EEPROM address for write */

//         I2C_Send7bitAddress(EEPROM_I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter);
//     } while (!(I2C_ReadRegister(EEPROM_I2Cx, I2C_Register_SR1) & 0x0002));

//     /* Clear AF flag */

//     I2C_ClearFlag(EEPROM_I2Cx, I2C_FLAG_AF);
//     /* STOP condition */

//     i2c_stop_on_bus(i2c0);
// }

/**
 * @brief  Basic management of the timeout situation.
 * @param  errorCode：错误代码，可以用来定位是哪个环节出错.
 * @retval 返回0，表示IIC读取失败.
 */
static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
    /* Block communication and all processes */
    I2C_ERROR("I2C 等待超时!errorCode = %d", errorCode);

    return 0;
}

// #define I2C0_SLAVE_ADDRESS7    0x72
// #define I2C1_SLAVE_ADDRESS7    0x82

// uint8_t i2c_receiver[16];

// void i2c0_rec_1type()
// {
//    int i = 0;
//     /* wait until I2C bus is idle */
//     while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
//     /* send a start condition to I2C bus */
//     i2c_start_on_bus(I2C0);
//     /* wait until SBSEND bit is set */
//     while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
//     /* send slave address to I2C bus */
//     i2c_master_addressing(I2C0, I2C1_SLAVE_ADDRESS7, I2C_RECEIVER);
//     /* wait until ADDSEND bit is set */
//     while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));

//     /* N=1,reset ACKEN bit before clearing ADDRSEND bit */
//     i2c_ack_config(I2C0, I2C_ACK_DISABLE);
//     /* clear ADDSEND bit */
//     i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
//     /* N=1,send stop condition after clearing ADDRSEND bit */
//     i2c_stop_on_bus(I2C0);
//     /* wait until the RBNE bit is set */
//     while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
//     /* read a data from I2C_DATA */
//     i2c_receiver[i++] = i2c_data_receive(I2C0);
//     /* wait until stop condition generate */
//     while(I2C_CTL0(I2C0) & I2C_CTL0_STOP);
//     /* enable acknowledge */
//     i2c_ack_config(I2C0, I2C_ACK_ENABLE);

//     printf("0x%02X ", i2c_receiver[0]);
// }

/**************************************************************
 * 函数名称: iic_config
 * 函数功能: iic配置
 * 输入参数: i2c_periph: I2C0~2
 *         : i2c_addr  : i2c地址
 * 输出参数: 无
 * 修改时间: 2022-9-01
 * ***********************************************************/
void iic_config(uint32_t i2c_periph, uint8_t i2c_addr)
{
    i2c_deinit(i2c_periph);
    iic_init(i2c_periph);

    /* i2c 时钟初始化 速度 100K */
    i2c_clock_config(i2c_periph, IIC_CLOCK_SPEED(100), I2C_DTCY_2);

    /* 设置 7bit 地址 */
    i2c_mode_addr_config(i2c_periph, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, i2c_addr);

    /* I2C 使能 */
    i2c_enable(i2c_periph);

    /* ACK 使能 */
    i2c_ack_config(i2c_periph, I2C_ACK_ENABLE);
}
/**************************************************************
 * 函数名称: iic_nvic_config
 * 函数功能: iic中断配置
 * 输入参数: i2c_periph: I2C0~2
 * 输出参数: 无
 * 修改时间: 2022-9-01
 * ***********************************************************/
void iic_nvic_config(uint32_t i2c_periph)
{
    uint8_t nvic_irq = 0;
    uint8_t nvic_irq_pre_priority = 0;
    uint8_t nvic_irq_sub_priority = 0;

    /* 中断设置 */
    switch (i2c_periph)
    {
    case I2C0:
        nvic_irq = I2C0_EV_IRQn;
        nvic_irq_pre_priority = 6;
        nvic_irq_sub_priority = 0;
        break;
    case I2C1:
        nvic_irq = I2C1_EV_IRQn;
        nvic_irq_pre_priority = 6;
        nvic_irq_sub_priority = 1;
        break;
    case I2C2:
        nvic_irq = I2C2_EV_IRQn;
        nvic_irq_pre_priority = 6;
        nvic_irq_sub_priority = 2;
        break;
    default:
        break;
    }
    nvic_irq_enable(nvic_irq, nvic_irq_pre_priority, nvic_irq_sub_priority);
    nvic_irq_enable(nvic_irq + 1, nvic_irq_pre_priority, nvic_irq_sub_priority); /* I2Cx_ER_IRQn */

    /* 中断使能 */
    i2c_interrupt_enable(i2c_periph, I2C_INT_EV);
    i2c_interrupt_enable(i2c_periph, I2C_INT_ERR);
    i2c_interrupt_enable(i2c_periph, I2C_INT_BUF);
}
/**************************************************************
 * 函数名称: iic_mode_switch
 * 函数功能: iic模式选择
 * 输入参数: i2c_periph: I2C0~2
 *         : mode      : iic工作模式
 * 输出参数: 无
 * 修改时间: 2022-9-01
 * ***********************************************************/
void iic_mode_switch(uint32_t i2c_periph, iic_workmode mode)
{
    switch (mode)
    {
    case iic_SendMode:
    {
        iic_config(i2c_periph, IIC_DEFAULT_ADDR);
    }
    break;

    case iic_RecvMode:
    {
        iic_config(i2c_periph, IIC_DEFAULT_ADDR);
        iic_nvic_config(i2c_periph);
    }
    break;

    default:
        break;
    }
}
/**************************************************************
 * 函数名称: iic_state_check
 * 函数功能: iic状态检测
 * 输入参数: u_char *caller1 一级调用函数
 *           u_char *caller2 二级调用函数
 *           uint32_t i2c_periph
 *           uint16_t flag   状态
 *           uint8_t timeout 超时时间
 * 输出参数: 无
 * 修改时间: 2022-9-05
 * ***********************************************************/
iic_state iic_state_check(char *caller1, char *caller2,
                          uint32_t i2c_periph, i2c_flag_enum flag, uint16_t timeout)
{
    /* 状态为1 */
    if ((flag == I2C_FLAG_SBSEND) ||
        (flag == I2C_FLAG_ADDSEND) ||
        (flag == I2C_FLAG_TBE) ||
        (flag == I2C_FLAG_BTC) ||
        (flag == I2C_FLAG_RBNE))
    {
        while (!i2c_flag_get(i2c_periph, flag))
        {
            delay_ms(1);
            // vTaskDelay(1);
            if (timeout-- == 0)
            {
                i2c_stop_on_bus(i2c_periph); // 总线结束
                bmc_debug(1, " %s %s %s flag = %#x error = %#x \n",
                          caller1, caller2, __func__, flag, (flag + IIC_ERR_OFFSET) & 0xff);

                return IIC_FLAG_TIMEOUT_ERR;
            }
        }
    }
    else if (flag == I2C_FLAG_I2CBSY)
    {
        while (i2c_flag_get(i2c_periph, flag))
        {
            delay_ms(1);
            // vTaskDelay(1);
            if (timeout-- == 0)
            {
                i2c_stop_on_bus(i2c_periph); // 总线结束
                bmc_debug(1, " %s %s %s flag = %#x error = %#x \n",
                          caller1, caller2, __func__, flag, (flag + IIC_ERR_OFFSET * 2) & 0xff);

                return IIC_FLAG_TIMEOUT_ERR;
            }
        }
    }
    else if (flag == I2C_CTL0_STOP)
    {
        while (I2C_CTL0(i2c_periph) & IIC_STOP_MASK)
        {
            delay_ms(1);

            if (timeout-- == 0)
            {
                i2c_stop_on_bus(i2c_periph); // 总线结束
                bmc_debug(1, " %s %s %s flag = %#x error = %#x\n",
                          caller1, caller2, __func__, flag, (flag + IIC_ERR_OFFSET) & 0xff);

                return IIC_FLAG_TIMEOUT_ERR;
            }
        }
    }
    else
    {
        i2c_stop_on_bus(i2c_periph); // 总线结束
        bmc_debug(1, " %s %s %s error flag = %#x\n", caller1, caller2, __func__, flag);

        return IIC_FLAG_WITHOUT_ERR;
    }
    return IIC_FLAG_OK;
}
/**************************************************************
 * 函数名称: iic_start
 * 函数功能: iic开始信号
 * 输入参数: uint32_t i2c_periph
 * 输出参数: iic_state
 * 修改时间: 2022-9-01 / 9-05
 * ***********************************************************/
iic_state iic_start(char *caller, uint32_t i2c_periph)
{
    char caller2[10] = "iic_start";
    uint16_t timeouts = 500;

    // while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY))
    //     ;

    i2c_start_on_bus(i2c_periph); // 发起 IIC start 起始位

    return iic_state_check(caller, caller2, i2c_periph, I2C_FLAG_SBSEND, timeouts);
}
/**************************************************************
 * 函数名称: i2c_stop
 * 函数功能: i2c停止信号
 * 输入参数: uint32_t i2c_periph
 * 输出参数: i2c_state
 * 修改时间: 2022-9-01 / 9-05
 * ***********************************************************/
iic_state iic_stop(char *caller, uint32_t i2c_periph)
{
    char caller2[9] = "iic_stop";

    i2c_stop_on_bus(i2c_periph); // 总线结束

    return iic_state_check(caller, caller2, i2c_periph, I2C_CTL0_STOP, 10);
}
/**************************************************************
 * 函数名称: iic_write_addr
 * 函数功能: iic写地址
 * 输入参数: uint32_t i2c_periph
 *           uint8_t addr
 *           uint32_t trandirection
 * 输出参数: iic_state
 * 修改时间: 2022-9-01 / 9-05
 * ***********************************************************/
iic_state iic_write_addr(char *caller, uint32_t i2c_periph, uint8_t addr, uint32_t trandirection)
{
    iic_state ret = IIC_FLAG_OK;
    char caller2[15] = "iic_write_addr";
    uint16_t timeouts = 500;

    i2c_master_addressing(i2c_periph, addr, trandirection); // 写地址寄存器
    ret = iic_state_check(caller, caller2, i2c_periph, I2C_FLAG_ADDSEND, timeouts);
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);
    if (ret)
        return ret;
    ret = iic_state_check(caller, caller2, i2c_periph, I2C_FLAG_TBE, timeouts);

    return ret;
}
/**************************************************************
 * 函数名称: iic_read_addr
 * 函数功能: iic读地址
 * 输入参数: uint32_t i2c_periph
 *           uint8_t addr
 *           uint32_t trandirection
 * 输出参数: iic_state
 * 修改时间: 2022-9-01 / 9-05
 * ***********************************************************/
iic_state iic_read_addr(char *caller, uint32_t i2c_periph, uint8_t addr, uint32_t trandirection)
{
    iic_state ret = IIC_FLAG_OK;
    char caller2[14] = "iic_read_addr";
    uint16_t timeouts = 500;

    i2c_master_addressing(i2c_periph, addr, trandirection); // 写寄存器
    ret = iic_state_check(caller, caller2, i2c_periph, I2C_FLAG_ADDSEND, timeouts);
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);
    if (ret)
        return ret;
    ret = iic_state_check(caller, caller2, i2c_periph, I2C_FLAG_RBNE, timeouts);

    return ret;
}
/**************************************************************
 * 函数名称: i2c_write_byte
 * 函数功能: i2c写字节
 * 输入参数: uint32_t i2c_periph
 *           uint8_t byte
 * 输出参数: i2c_state
 * 修改时间: 2022-9-01 / 9-05
 * ***********************************************************/
iic_state iic_write_byte(char *caller, uint32_t i2c_periph, uint8_t byte)
{
    char caller2[15] = "iic_write_byte";
    uint16_t timeouts = 500;

    i2c_data_transmit(i2c_periph, byte);

    return iic_state_check(caller, caller2, i2c_periph, I2C_FLAG_BTC, timeouts);
}
/**************************************************************
 * 函数名称: iic_read_byte
 * 函数功能: iic读字节
 * 输入参数: uint32_t i2c_periph
 *           uint8_t ack 应答信号
 * 输出参数: uint8_t 读到的字节
 * 修改时间: 2022-9-01 / 9-05
 * ***********************************************************/
uint8_t iic_read_byte(char *caller, uint32_t i2c_periph, uint8_t ack)
{
    uint8_t res = 0;

    res = i2c_data_receive(i2c_periph);

    if (ack)
    { // ACK
        i2c_ack_config(i2c_periph, I2C_ACK_ENABLE);
    }
    else
    { // NACK
        i2c_ack_config(i2c_periph, I2C_ACK_DISABLE);
    }
    bmc_debug(1, " %s %s res = %#x\n", caller, __func__, res);

    return res;
}
/**************************************************************
 * 函数名称: iic_event_irq_handle
 * 函数功能: iic 事件中断处理函数
 * 输入参数: uint32_t i2c_periph
 * 输出参数: uint8_t 读到的字节
 * 修改时间: 2022-9-28
 * ***********************************************************/
void iic_event_irq_handle(uint32_t i2c_periph)
{
    if (i2c_interrupt_flag_get(i2c_periph, I2C_INT_FLAG_ADDSEND))
    {
        /* clear the ADDSEND bit */
        i2c_interrupt_flag_clear(i2c_periph, I2C_INT_FLAG_ADDSEND);
    }
    else if (i2c_interrupt_flag_get(i2c_periph, I2C_INT_FLAG_RBNE))
    {
        /* if reception data register is not empty ,I2C1 will read a data from I2C_DATA */
        gi2c_recv = i2c_data_receive(i2c_periph);

        i2c_interrupt_flag_clear(i2c_periph, I2C_INT_FLAG_RBNE);
    }
    else if (i2c_interrupt_flag_get(i2c_periph, I2C_INT_FLAG_STPDET))
    {
        /* clear the STPDET bit */
        i2c_enable(i2c_periph);

        i2c_interrupt_flag_clear(i2c_periph, I2C_INT_FLAG_STPDET);
    }
    bmc_debug(gi2c_recv, " %s gi2c_recv = %#x \n", __func__, gi2c_recv);
}
/**************************************************************
 * 函数名称: iic_error_irq_handle
 * 函数功能: iic 错误标志中断处理函数
 * 输入参数: uint32_t i2c_periph
 * 输出参数: uint8_t 读到的字节
 * 修改时间: 2022-9-28
 * ***********************************************************/
void iic_error_irq_handle(uint32_t i2c_periph)
{
    /* no acknowledge received */
    if (i2c_interrupt_flag_get(i2c_periph, I2C_INT_FLAG_AERR))
    {
        i2c_interrupt_flag_clear(i2c_periph, I2C_INT_FLAG_AERR);
    }

    /* SMBus alert */
    if (i2c_interrupt_flag_get(i2c_periph, I2C_INT_FLAG_SMBALT))
    {
        i2c_interrupt_flag_clear(i2c_periph, I2C_INT_FLAG_SMBALT);
    }

    /* bus timeout in SMBus mode */
    if (i2c_interrupt_flag_get(i2c_periph, I2C_INT_FLAG_SMBTO))
    {
        i2c_interrupt_flag_clear(i2c_periph, I2C_INT_FLAG_SMBTO);
    }

    /* over-run or under-run when SCL stretch is disabled */
    if (i2c_interrupt_flag_get(i2c_periph, I2C_INT_FLAG_OUERR))
    {
        i2c_interrupt_flag_clear(i2c_periph, I2C_INT_FLAG_OUERR);
    }

    /* arbitration lost */
    if (i2c_interrupt_flag_get(i2c_periph, I2C_INT_FLAG_LOSTARB))
    {
        i2c_interrupt_flag_clear(i2c_periph, I2C_INT_FLAG_LOSTARB);
    }

    /* bus error */
    if (i2c_interrupt_flag_get(i2c_periph, I2C_INT_FLAG_BERR))
    {
        i2c_interrupt_flag_clear(i2c_periph, I2C_INT_FLAG_BERR);
    }

    /* CRC value doesn't match */
    if (i2c_interrupt_flag_get(i2c_periph, I2C_INT_FLAG_PECERR))
    {
        i2c_interrupt_flag_clear(i2c_periph, I2C_INT_FLAG_PECERR);
    }

    /* disable the error interrupt */
    i2c_interrupt_disable(i2c_periph, I2C_INT_ERR);
    i2c_interrupt_disable(i2c_periph, I2C_INT_EV);
    i2c_interrupt_disable(i2c_periph, I2C_INT_BUF);
}

// #define I2C_10BIT_ADDRESS 0
// #define I2C0_SLAVE_ADDRESS10 0x0322
// uint8_t i2c_receiver[16];

// #define I2C0_OWN_ADDRESS7 0x72
// #define I2C0_SLAVE_ADDRESS7 0x43
// uint8_t i2c_transmitter[16];

// /* 主机发送 */
// void masterSend(void)
// {
//     /* 初始化并填充数据0x80-0x8F */
//     int i;
//     for (i = 0; i < 16; i++)
//     {
//         i2c_transmitter[i] = i + 0x80;
//     }

//     printf(" iic bus start \r\n");

//     /* 等待总线空闲 RESET = 0; SET = !RESET = 1 */
//     while (!RESET == i2c_flag_get(I2C0, I2C_FLAG_I2CBSY))
//     {
//         i2c_stop_on_bus(I2C0);
//         // printf("iic bus not idle\r\n");
//     };
//     /* 发送开始信号 */
//     i2c_start_on_bus(I2C0);
//     /* 等待开始信号发送成功,被设置则发送成功 */
//     while (RESET == !i2c_flag_get(I2C0, I2C_FLAG_SBSEND))
//     {
//         i2c_stop_on_bus(I2C0);
//         // printf("iic bus not start\r\n");
//     };
//     /* 发送从设备地址 */
//     i2c_master_addressing(I2C0, I2C0_SLAVE_ADDRESS7, I2C_TRANSMITTER);
//     /* 等待发送成功标志 */
//     while (RESET == !i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
//     {
//         i2c_stop_on_bus(I2C0);
//         // printf("slave addr not send\r\n");
//     };
//     /* 清除标志 */
//     i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
//     /* 等待传输数据缓冲区为空 */
//     while (RESET == !i2c_flag_get(I2C0, I2C_FLAG_TBE))
//     {
//         // i2c_stop_on_bus(I2C0);
//         // printf("rec buffer non-empty\r\n");
//     };

//     for (i = 0; i < 16; i++)
//     {
//         /* 数据发送 */
//         i2c_data_transmit(I2C0, i2c_transmitter[i]);
//         /* 等待发送标志位 */
//         while (RESET == !i2c_flag_get(I2C0, I2C_FLAG_TBE))
//         {
//             i2c_stop_on_bus(I2C0);
//             // printf("send type TBE busy\r\n");
//         };
//     }
//     /* 发送停止信号 */
//     i2c_stop_on_bus(I2C0);
//     while (!RESET == I2C_CTL0(I2C0) & I2C_CTL0_STOP)
//     {
//         // i2c_stop_on_bus(I2C0);
//         // printf("iic bus exit\r\n");
//     };

//     // iic_read_addr(caller, I2C0, 0x11, I2C_RECEIVER);

// }

// /* 主机接收  */
// void masterRec(void)
// {
//     uint8_t i;
// #if I2C_10BIT_ADDRESS
//     uint8_t slave10_first_byte, slave10_second_byte; // 10位地址拆为两部分
// #endif

//     printf("\r\n I2C Start\r\n");

//     /* 等待总线空闲 */
//     while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY))
//         ;
//     /* 发送起始信号 */
//     i2c_start_on_bus(I2C0);
//     /* 等待SBSEND置1 确保起始信号发送成功 */
//     while (!i2c_flag_get(I2C0, I2C_FLAG_SBSEND))
//         ;

// #if I2C_10BIT_ADDRESS
//     slave10_first_byte = (0xF0) | (uint8_t)((I2C0_SLAVE_ADDRESS10 & 0x0300) >> 7);
//     /* 发送10位地址第1部分 */
//     i2c_master_addressing(I2C0, slave10_first_byte, I2C_TRANSMITTER);
//     /* 等待ADD10SEND被设置 发送成功的信号 */
//     while (!i2c_flag_get(I2C0, I2C_FLAG_ADD10SEND))
//         ;
//     /* 传递第2部分剩下的8位 */
//     slave10_second_byte = (uint8_t)(I2C0_SLAVE_ADDRESS10 & 0x00FF);
//     /* 发送第2部分 */
//     i2c_master_addressing(I2C0, slave10_second_byte, I2C_TRANSMITTER);
//     /* 等待发送成功标志 */
//     while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
//         ;
//     /* 清除标志 */
//     i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
//     /* 发送开始信号 */
//     i2c_start_on_bus(I2C0);
//     /* 确认开始信号发送成功 */
//     while (!i2c_flag_get(I2C0, I2C_FLAG_SBSEND))
//         ;
//     /* 发送从机地址第1字节 */
//     i2c_master_addressing(I2C0, slave10_first_byte, I2C_RECEIVER);
// #else
//     /* 发送从机地址 */
//     i2c_master_addressing(I2C0, I2C0_SLAVE_ADDRESS7, I2C_RECEIVER);
// #endif
//     /* 等待发送成功标志 */
//     while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
//         ;
//     /* 清除标志 */
//     i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

//     for (i = 0; i < 16; i++)
//     {
//         if (13 == i)
//         {
//             /* 等待数据发送完成 */
//             while (!i2c_flag_get(I2C0, I2C_FLAG_BTC))
//                 ;
//             /* 禁用应答 */
//             i2c_ack_config(I2C0, I2C_ACK_DISABLE);
//         }
//         /* 等待接收成功标志位 */
//         while (!i2c_flag_get(I2C0, I2C_FLAG_RBNE))
//             ;
//         // 在这里处理数据
//         /* 读取数据 */
//         i2c_receiver[i] = i2c_data_receive(I2C0);
//     }
//     /* 发送停止条件 */
//     i2c_stop_on_bus(I2C0);
//     /* 等待停止条件生成 */
//     while (I2C_CTL0(I2C0) & I2C_CTL0_STOP)
//         ;
//     /* 使能应答 */
//     i2c_ack_config(I2C0, I2C_ACK_ENABLE);

//     for (i = 0; i < 16; i++)
//     {
//         printf("\r\nThe I2C Rec Data :0x%02X \r\n", i2c_receiver[i]);
//     }
// }

// /* 中断触发 */
// uint8_t data;

// void I2C0_EV_IRQHandler(void)
// {
//     if (i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
//     { // 从机地址匹配成功
//         i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
//         i2c_data_receive(I2C0); // 清除接收缓冲区
//     }

//     if (i2c_flag_get(I2C0, I2C_FLAG_RBNE))
//     { // 接收数据就绪
//         i2c_flag_clear(I2C0, I2C_FLAG_RBNE);
//         data = i2c_data_receive(I2C0); // 读取数据
//         usart0_send_byte(data);        // 回环
//     }

//     if (i2c_flag_get(I2C0, I2C_FLAG_STPDET))
//     { // 停止条件检测
//         i2c_flag_clear(I2C0, I2C_FLAG_STPDET);
//         i2c_disable(I2C0); // 关闭I2C以重新初始化
//         i2c_enable(I2C0);  // 重新使能I2C
//     }
//     if (i2c_flag_get(I2C0, I2C_FLAG_AERR))
//     { // 检测NACK
//         i2c_flag_clear(I2C0, I2C_FLAG_AERR);
//         // 重试或报错
//         // printf("NACK detected!\r\n");
//     }
// }

void I2C_Send_Data(unsigned char slaveadr, unsigned char data)
{
    while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY))
        ;

    i2c_start_on_bus(I2C0); // 起始信号

    while (!i2c_flag_get(I2C0, I2C_FLAG_SBSEND))
        ;

    i2c_master_addressing(I2C0, slaveadr, I2C_TRANSMITTER); // 从机地址

    while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
        ;

    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND); // 主机地址从机响应

    while (SET != i2c_flag_get(I2C0, I2C_FLAG_TBE))
        ; // IIC_DATA在传输时为空

    i2c_data_transmit(I2C0, data);

    while (!i2c_flag_get(I2C0, I2C_FLAG_BTC))
        ; // 字节传输完成

    i2c_stop_on_bus(I2C0);

    while (I2C_CTL0(I2C0) & 0x0200)
        ;
}
