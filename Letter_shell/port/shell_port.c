/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

//#include "gd32f4xx.h"

#include "shell_port.h"
#include "main.h"

/* 1. 创建shell对象，开辟shell缓冲区 */
extern Shell shell;
char shell_buffer[512];
 
 
 
/* 2. 自己实现shell写函数 */
 
//shell写函数原型：typedef void (*shellWrite)(const char);

signed short User_Shell_Write(char* data,unsigned short len)
{
	int i;
	
	for(i=0;i<len;i++){
		usart_data_transmit(USART0, (uint8_t) data[i]);
		/* 等待发送完毕 */
		while (usart_flag_get(USART0, USART_FLAG_TBE) == RESET); 		
	}
 
	return 0; 	
}
 
 
/* 3. 编写初始化函数 */
void User_Shell_Init(void)
{
	//注册自己实现的写函数
    shell.write = User_Shell_Write;
	
	//调用shell初始化函数
    shellInit(&shell,shell_buffer,sizeof(shell_buffer));
}


