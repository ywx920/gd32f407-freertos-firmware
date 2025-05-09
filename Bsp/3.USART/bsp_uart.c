#include "bsp_uart.h"
#include "shell_port.h"

extern uint8_t recv_buf_shell;
extern Shell shell;

volatile uint8_t usart0_tx_buffer[128]; // 发送缓冲区
volatile uint16_t usart0_tx_head = 0, usart0_tx_tail = 0;

uint32_t int_flag;
uint16_t next_head;

#if 1 //-----------------------------------------------------------------
// cortex-m 内核在 MDK 中对 printf 函数进行了优化导致处理时需要配置
// 方法两种：1.用如下方法进行处理；2.用MDK自带的微库处理
// 加入以下代码,支持 printf 函数
#pragma import(__use_no_semihosting)
// 标准库需要支持的函数
struct __FILE
{
    int handle;
};

FILE __stdout;

// 定义 _sys_exit() 以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
#endif //-----------------------------------------------------------------

/* 重新定义 fputc 函数 */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while (RESET == usart_flag_get(USART0, USART_FLAG_TBE))
        ;
    return ch;
}

int fgetc(FILE *f)
{
    /* 等待串口0输入数据 */
    while (usart_flag_get(USART0, USART_FLAG_RBNE) == RESET)
        ;

    return (int)usart_data_receive(USART0);
}

/*
    usart0----mcu调试串口
    usart1----cpu通信串口
    usart2----fpga通信串口
    uart3-----预留通信串口

    MCU：
    [MCU-USB]---------[MCU_USART1]
    MCU_USART1_TX-----PA9 ---USART0_TX
	MCU_USART1_RX-----PA10---USART0_RX

    [CPU-RS232-J33]---[MCU_USART2]
    CPU_UART0_RXD-----MCU_USART2_TX---PD5---USART1_TX
    CPU_UART0_TXD-----MCU_USART2_RX---PD6---USART1_RX

    [FPGA]------------[MCU_USART3]
    FPGA_UART_RX------MCU_USART3_TX---PB10---USART2_TX
    FPGA_UART_TX------MCU_USART3_RX---PB11---USART2_RX

    [MCU-RS232-J34]---[MCU_UART4]
    MCU_UART4_TX------PC10---UART3_TX
	MCU_UART4_RX------PC11---UART3_RX
    
*/
void uart_init(uint32_t usart_periph, uint32_t baudval)
{
    switch (usart_periph)
    {
    case USART0:
    {
        /***时钟初始化***/
        rcu_periph_clock_enable(RCU_GPIOA);
        rcu_periph_clock_enable(RCU_USART0);

        /***引脚模式初始化***/
        gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9 | GPIO_PIN_10);
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9 | GPIO_PIN_10);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9 | GPIO_PIN_10);

        /***中断配置***/
        nvic_irq_enable(USART0_IRQn, 5, 0);
        break;
    }
    case USART1:
    {
        /***时钟初始化***/
        rcu_periph_clock_enable(RCU_GPIOD);
        rcu_periph_clock_enable(RCU_USART1);

        /***引脚模式初始化***/
        gpio_af_set(GPIOD, GPIO_AF_7, GPIO_PIN_5 | GPIO_PIN_6);
        gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_5 | GPIO_PIN_6);
        gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_6);

        /***中断配置***/
        nvic_irq_enable(USART1_IRQn, 5, 0);
        break;
    }
    case USART2:
    {
        /***时钟初始化***/
        rcu_periph_clock_enable(RCU_GPIOB);
        rcu_periph_clock_enable(RCU_USART2);

        /***引脚模式初始化***/
        gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_10 | GPIO_PIN_11);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10 | GPIO_PIN_11);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_11);

        /***中断配置***/
        nvic_irq_enable(USART2_IRQn, 5, 0);
        break;
    }
    //备用
    case UART3:
    {
        /***时钟初始化***/
        rcu_periph_clock_enable(RCU_GPIOC);
        rcu_periph_clock_enable(RCU_UART3);

        /***引脚模式初始化***/
        gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_10 | GPIO_PIN_11);
        gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10 | GPIO_PIN_11);
        gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_11);

        /***中断配置***/
        nvic_irq_enable(UART3_IRQn, 5, 0);
    }
    default:
        return;
    }

    /* 串口 配置 */
    usart_deinit(usart_periph);
    usart_baudrate_set(usart_periph, baudval);                       // 波特率
    usart_word_length_set(usart_periph, USART_WL_8BIT);              // 数据位
    usart_stop_bit_set(usart_periph, USART_STB_1BIT);                // 停止位
    usart_parity_config(usart_periph, USART_PM_NONE);                // 奇偶校验 - 无
    usart_hardware_flow_rts_config(usart_periph, USART_RTS_DISABLE); // rts - 无
    usart_hardware_flow_cts_config(usart_periph, USART_CTS_DISABLE); // cts - 无
    usart_receive_config(usart_periph, USART_RECEIVE_ENABLE);        // 接收 使能
    usart_transmit_config(usart_periph, USART_TRANSMIT_ENABLE);      // 发送 使能
    usart_enable(usart_periph);                                      // 使能 串口

    /* 使能接收中断 */
    usart_interrupt_enable(usart_periph, USART_INT_RBNE);
}

static inline void Send_char(uint32_t usart_periph, uint8_t ch)
{
    usart_data_transmit(usart_periph, (uint8_t)ch);
    while (RESET == usart_flag_get(usart_periph, USART_FLAG_TC))
        ;
}

void send_string(uint8_t *buf, uint16_t strlen, uint32_t usart_periph)
{
    // if (usart_periph == USART1)
    // {
    //     gpio_bit_set(GPIOD, GPIO_PIN_5);
    //     gpio_bit_set(GPIOD, GPIO_PIN_6);
    //     for (uint16_t i = 0; i <= strlen; i++)
    //     {
    //         Send_char(usart_periph, buf[i]);
    //     }
    //     gpio_bit_reset(GPIOD, GPIO_PIN_5);
    //     gpio_bit_reset(GPIOD, GPIO_PIN_6);
    // }
    // else
    {
        for (uint16_t i = 0; i <= strlen; i++)
        {
            Send_char(usart_periph, buf[i]);
        }
    }
}

uart_data_t data_uart[5] = {
    [0].gvUsartRxFlag = 1,
    [0].gvUsartRxLen = 0,
    [1].gvUsartRxFlag = 1,
    [1].gvUsartRxLen = 0,
    [2].gvUsartRxFlag = 1,
    [2].gvUsartRxLen = 0,
    [3].gvUsartRxFlag = 1,
    [3].gvUsartRxLen = 0,
    [4].gvUsartRxFlag = 1,
    [4].gvUsartRxLen = 0,
};

/*****************************************************************************
 * 函数名称：usartReceiveIrq
 * 函数功能：中断法完成对板卡某个串口接收数据
 * 输入参数：vUsartX为串口号
 * 返回值：无
 * 输入全局变量：无
 * 输出全局变量：gvUsartRxBuff: 串口接收的数据
 * 创建者与创建日期：lht 2022-8-26
 **************************************************************************/
uart_data_t *data_temp;
void usart_recv_irq(uint32_t usart_x)
{
    uint8_t Rxc = 0;
    // printf("%s \n",__func__);

    switch (usart_x)
    {
    case USART0:
    {
        if (RESET != usart_interrupt_flag_get(usart_x, USART_INT_FLAG_RBNE))
        {
            usart_interrupt_flag_clear(usart_x, USART_INT_FLAG_RBNE);
            // 使能串口中断接收
            recv_buf_shell = usart_data_receive(usart_x);
            
            // 调用shell处理数据的接口
            // printf("received data is %x\r\n",recv_buf_shell);
            shellHandler(&shell, recv_buf_shell);
        }
        break;
    }
    case USART1:
    {
        data_temp = &data_uart[1];
        break;
    }
    case USART2:
    {
        data_temp = &data_uart[2];
        break;
    }
    case UART3:
    {
        data_temp = &data_uart[3];
        break;
    }
    }
    if ((usart_x != USART0))
    {
        if (data_temp->gvUsartRxFlag)
        {
            if (RESET != usart_interrupt_flag_get(usart_x, USART_INT_FLAG_RBNE))
            {
                usart_interrupt_flag_clear(usart_x, USART_INT_FLAG_RBNE);
                Rxc = usart_data_receive(usart_x);
								// printf("rx:%x\r\n", Rxc);
                data_temp->gvUsartRxBuff[data_temp->gvUsartRxLen % UARTBUFF_SIZE] = Rxc;
                data_temp->gvUsartRxLen = (data_temp->gvUsartRxLen + 1) % UARTBUFF_SIZE;
            }
        }
    }
}


void usart0_send_byte(uint8_t data)
{
    int_flag = __get_PRIMASK(); // 保存中断状态
    __disable_irq();

    next_head = (usart0_tx_head + 1) % sizeof(usart0_tx_buffer);
    if (next_head != usart0_tx_tail)
    {
        usart0_tx_buffer[usart0_tx_head] = data;
        usart0_tx_head = next_head;
        usart_interrupt_enable(USART0, USART_INT_TBE); // 触发发送中断
    }

    __set_PRIMASK(int_flag); // 恢复中断状态
}
