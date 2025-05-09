#include "bsp_Control_xc6s.h"

uint8_t fanrunmode = 0;//风扇运行模式;0自动;1手动


// void uart3_init(uint32_t baudval)
// {
//     /* 使能引脚时钟 */
//     rcu_periph_clock_enable(RCU_GPIOC);

//     /* 使能串口时钟 */
//     rcu_periph_clock_enable(RCU_UART3);

//     /* 初始化 引脚 */
//     gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
//     gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

//     /* 串口 配置 */
//     usart_deinit(UART3);
//     usart_baudrate_set(UART3, baudval);                 //波特率
//     usart_word_length_set(UART3, USART_WL_8BIT);        //数据位
//     usart_stop_bit_set(UART3, USART_STB_2BIT);          //停止位
//     usart_parity_config(UART3, USART_PM_NONE);          //奇偶校验 - 无
//     usart_hardware_flow_rts_config(UART3, USART_RTS_DISABLE);//rts - 无
//     usart_hardware_flow_cts_config(UART3, USART_CTS_DISABLE);//cts - 无
//     usart_receive_config(UART3, USART_RECEIVE_ENABLE);       //接收 使能
//     usart_transmit_config(UART3, USART_TRANSMIT_ENABLE);     //发送 使能
//     usart_enable(UART3);                                     //使能 串口
    
//     /* 串口中断优先级配置 */
//     nvic_irq_enable(UART3_IRQn, 5, 0);
    
//     /* 使能接收中断 */
//     usart_interrupt_enable(UART3, USART_INT_RBNE);
// }

void uart3Send_char(uint8_t ch)
{
    usart_data_transmit(UART3, (uint8_t)ch);
    while(RESET == usart_flag_get(UART3, USART_FLAG_TBE));
}


//风扇等级控制
void fan_level(uint8_t level)
{
    if((level >=40 )&&(level <= 100))
    {
        uart3Send_char(level);
    }
}


//风扇模式
void setfanMode(uint8_t mode)
{
    fanrunmode = mode;
}

//风扇运行模式;0自动;1手动
uint8_t getFanMode(void)
{
    return fanrunmode;
}
