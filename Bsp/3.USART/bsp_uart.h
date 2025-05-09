#ifndef _BSP_UART_H_
#define _BSP_UART_H_

#include "main.h"

#define UARTBUFF_SIZE   512

typedef struct uart_data {
	uint8_t gvUsartRxBuff[UARTBUFF_SIZE];
	uint8_t gvUsartRxFlag;
	uint8_t volatile gvUsartRxLen;
} uart_data_t;

void uart_init(uint32_t usart_periph,uint32_t baudval);

void Send_char(uint32_t usart_periph,uint8_t ch);

void send_string(uint8_t *buf,uint16_t strlen,uint32_t usart_periph);

void usart_recv_irq(uint32_t usart_x);

#endif //_BSP_UART_H_
