/**************************************************************
 * 版权    : ZONKIN @2022
 * 文件名  : bsp_i2c.c
 * 文件说明: i2c配置文件
 * 创建人  : lht 2022-9-01
 * 修改时间:
 * ***********************************************************/

#include "bsp_i2c.h"

uint8_t gi2c_recv = 0;

/*
    PB5---I2C0_SMBA   CPU_I2C0_ALERT_N   SMB_ALERT
    PB6---I2C0_SCL    CPU_I2C0_SCL       SMB_CK
    PB7---I2C0_SDA    CPU_I2C0_SDA       SMB_DAT

    PF0---I2C1_SDA    CPU_I2C1_SDA       I2C_DAT
    PF1---I2C1_SCL    CPU_I2C1_SCL       I2C_CK

    PC9---I2C2_SDA
    PA8---I2C2_SCL
*/
#define I2C_10BIT_ADDRESS      0
#define I2C0_SLAVE_ADDRESS10   0x0322
uint8_t i2c_receiver[16];

#define I2C0_OWN_ADDRESS7 0x72
#define I2C0_SLAVE_ADDRESS7 0x82
uint8_t i2c_transmitter[16];

/* 主机发送 */
void masterSend(void)
{
    int i;
    for (i = 0; i < 16; i++)
    {
        i2c_transmitter[i] = i + 0x80;
    }

    /* 等待总线空闲 */
    while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY))
        ;
    /* 发送开始信号 */
    i2c_start_on_bus(I2C0);
    /* 等待开始信号发送成功 */
    while (!i2c_flag_get(I2C0, I2C_FLAG_SBSEND))
        ;
    /* 发送从设备地址 */
    i2c_master_addressing(I2C0, I2C0_SLAVE_ADDRESS7, I2C_TRANSMITTER);
    /* 等待发送成功标志 */
    while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
        ;
    /* 清除标志 */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    /* 等待传输数据缓冲区为空 */
    while (!i2c_flag_get(I2C0, I2C_FLAG_TBE))
        ;

    for (i = 0; i < 16; i++)
    {
        /* 数据发送 */
        i2c_data_transmit(I2C0, i2c_transmitter[i]);
        /* 等待发送标志位 */
        while (!i2c_flag_get(I2C0, I2C_FLAG_TBE))
            ;
    }
    /* 发送停止信号 */
    i2c_stop_on_bus(I2C0);
    while (I2C_CTL0(I2C0) & I2C_CTL0_STOP)
        ;
}

/* 主机接收  */
void masterRec(void)
{
    uint8_t i;
#if I2C_10BIT_ADDRESS
    uint8_t slave10_first_byte, slave10_second_byte; //10位地址拆为两部分
#endif

    printf("\r\n I2C Start\r\n"); 

    /* 等待总线空闲 */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
    /* 发送起始信号 */
    i2c_start_on_bus(I2C0);
    /* 等待SBSEND置1 确保起始信号发送成功 */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));

#if I2C_10BIT_ADDRESS
    slave10_first_byte = (0xF0) | (uint8_t)((I2C0_SLAVE_ADDRESS10 & 0x0300) >> 7);
    /* 发送10位地址第1部分 */
    i2c_master_addressing(I2C0, slave10_first_byte, I2C_TRANSMITTER);
    /* 等待ADD10SEND被设置 发送成功的信号 */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADD10SEND));
    /* 传递第2部分剩下的8位 */
    slave10_second_byte = (uint8_t)(I2C0_SLAVE_ADDRESS10 & 0x00FF);
    /* 发送第2部分 */
    i2c_master_addressing(I2C0, slave10_second_byte, I2C_TRANSMITTER);
    /* 等待发送成功标志 */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    /* 清除标志 */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    /* 发送开始信号 */
    i2c_start_on_bus(I2C0);
    /* 确认开始信号发送成功 */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    /* 发送从机地址第1字节 */
    i2c_master_addressing(I2C0, slave10_first_byte, I2C_RECEIVER);
#else
    /* 发送从机地址 */
    i2c_master_addressing(I2C0, I2C0_SLAVE_ADDRESS7, I2C_RECEIVER);
#endif
    /* 等待发送成功标志 */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    /* 清除标志 */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

    for(i = 0; i < 16; i++) {
        if(13 == i) {
            /* 等待数据发送完成 */
            while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
            /* 禁用应答 */
            i2c_ack_config(I2C0, I2C_ACK_DISABLE);
        }
        /* 等待接收成功标志位 */
        while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
        /* 读取数据 */
        i2c_receiver[i] = i2c_data_receive(I2C0);
    }
    /* 发送停止条件 */
    i2c_stop_on_bus(I2C0);
    /* 等待停止条件生成 */
    while(I2C_CTL0(I2C0) & I2C_CTL0_STOP);
    /* 使能应答 */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);

    for(i = 0; i < 16; i++) {
        printf("\r\nThe I2C Rec Data :0x%02X \r\n", i2c_receiver[i]);
    }
}


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
        i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
        /* configure I2C address */
        i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_OWN_ADDRESS7);
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
        i2c_clock_config(I2C1, 100000, I2C_DTCY_2);
        /* configure I2C address */
        i2c_mode_addr_config(I2C1, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_OWN_ADDRESS7);// 地址待改
        /* enable I2C0 */
        i2c_enable(I2C1);
        /* enable acknowledge */
        i2c_ack_config(I2C1, I2C_ACK_ENABLE);

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
iic_state
iic_state_check(char *caller1, char *caller2,
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
            vTaskDelay(1);
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
            vTaskDelay(1);
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
            vTaskDelay(1);
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
