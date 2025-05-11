/*!
    \file    main.c
    \brief   running led

    \version 2024-01-15, V3.2.0, firmware for GD32F4xx
*/

#include "main.h"
// #include "gd32f4xx.h"
#include <stdio.h>
#include "systick.h"
#include "sys_config.h"
// #include "bsp_gpio.h"
// #include "bsp_nst175.h"
// #include "commcpu.h"
#include "bsp_adc.h"
// #include "delay.h"
#include "bsp_uart.h"
#include "bsp_i2c.h"
#include "shell.h"
#include "bsp_phy_YT8512H.h"
#include "lwip_client.h"

#include "gd32f4xx_i2c.h"
#include "gd32f4xx_gpio.h"

TaskHandle_t StartTask_Handler; // 启动任务句柄
BaseType_t Shell_task_Handle;
BaseType_t Temp_Task_Handle;
BaseType_t Adc_Task_Handle;
BaseType_t Led_Task_Handle;
BaseType_t Cpuinfo_Task_Handle;
BaseType_t ComCpu_Task_Handle;
BaseType_t Net_Task_Handle;
BaseType_t UdpServer_Task_Handle;
BaseType_t TcpClient_Task_Handle;

static void StarTask(void *pvParameters);
uint8_t recv_buf_shell = 0;
Shell shell;

void SysModule_Init(void)
{
  systick_config();
  SYS_INIT(120);

  /* 运行操作系统需要配置这等级 */
  // nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
  /* led初始化 */
  // led_init();
  // nst175_init();
  // adcinit();
  //  EEPROM_I2C_CONFIG();
  //		phy_init();
  /* 串口初始化 */
  uart_init(USART0, 115200);
  // uart_init(USART1, 115200);
  // uart_init(USART2, 115200);

  iic_init(I2C0);

  // User_Shell_Init();
}

void ShellCreateTask(void)
{
  Shell_task_Handle = xTaskCreate(shellTask, "SHELL", 1024, NULL, 1, NULL);
  if (NULL == Shell_task_Handle)
  {
    printf("SHELL TASK Create Error!");
  }
}

void TempCreateTask(void)
{
  taskENTER_CRITICAL(); /* 进入临界区 */
  Temp_Task_Handle = xTaskCreate(TempTask, "TEMP", 256, NULL, 2, NULL);
  if (NULL == Temp_Task_Handle)
  {
    printf("TEMP TASK Create Error!");
  }
  // vTaskDelete(StartTask_Handler); /* 删除开始任务 */
  taskEXIT_CRITICAL(); /* 推出临界区 */
}

void AdcCreateTask(void)
{
  taskENTER_CRITICAL(); /* 进入临界区 */
  Adc_Task_Handle = xTaskCreate(adcTask, "adc", 256, NULL, 2, NULL);
  if (NULL == Adc_Task_Handle)
  {
    printf("ADC TASK Create Error!");
  }
  taskEXIT_CRITICAL(); /* 推出临界区 */
}

void LedCreateTask(void)
{
  taskENTER_CRITICAL(); /* 进入临界区 */
  Led_Task_Handle = xTaskCreate(ledTASK, "LED", 256, NULL, 3, NULL);
  if (NULL == Led_Task_Handle)
  {
    printf("LED TASK Create Error!");
  }
  // vTaskDelete(StartTask_Handler); /* 删除开始任务 */
  taskEXIT_CRITICAL(); /* 推出临界区 */
}
void ComCpuCreateTask(void)
{
  taskENTER_CRITICAL(); /* 进入临界区 */
  ComCpu_Task_Handle = xTaskCreate(ComCpu1Task, "commcpu1", 512, NULL, 2, NULL);
  if (NULL == ComCpu_Task_Handle)
  {
    printf("COMCPU TASK Create Error!");
  }
  // vTaskDelete(StartTask_Handler); /* 删除开始任务 */
  taskEXIT_CRITICAL(); /* 推出临界区 */
}

void NetCreateTask(void)
{
  taskENTER_CRITICAL(); /* 进入临界区 */
  Net_Task_Handle = xTaskCreate(netTask, "net", 512, NULL, 2, NULL);
  if (NULL == Net_Task_Handle)
  {
    printf("NET TASK Create Error!");
  }
  taskEXIT_CRITICAL(); /* 推出临界区 */
}
void UdpServerCreateTask(void)
{
  taskENTER_CRITICAL(); /* 进入临界区 */
  UdpServer_Task_Handle = xTaskCreate(UdpServer, "UdpServer", 512, NULL, 2, NULL);
  if (NULL == UdpServer_Task_Handle)
  {
    printf("UdpServer TASK Create Error!");
  }
  taskEXIT_CRITICAL(); /* 推出临界区 */
}
void TcpClientCreateTask(void)
{
  taskENTER_CRITICAL(); /* 进入临界区 */
  TcpClient_Task_Handle = xTaskCreate(Tcp_client, "TcpClient", 512, NULL, 2, NULL);
  if (NULL == TcpClient_Task_Handle)
  {
    printf("TcpClient TASK Create Error!");
  }
  taskEXIT_CRITICAL(); /* 推出临界区 */
}

// void i2c_write(uint8_t reg,uint8_t data)
// {
//     char caller[13] = "i2c0_write";

//     //等待 IIC 空闲
//     iic_state_check(caller, caller, I2C0, I2C_FLAG_I2CBSY, 10);
//     iic_start(caller, I2C0);
//     iic_write_addr(caller, I2C0, I2C_SLAVE_BIT7, I2C_TRANSMITTER);
//     iic_write_byte(caller, I2C0, reg);
//     iic_write_byte(caller, I2C0, data);
//     iic_stop(caller, I2C0);

//     bmc_debug(1,"%s over\n",caller);
// }

// uint8_t i2c_read(uint8_t reg)
// {
//     uint8_t ret = 0;
//     char caller[12] = "i2c0_read";

//     //等待 IIC 空闲
//     iic_state_check(caller, caller, I2C0, I2C_FLAG_I2CBSY, 10);
//     iic_start(caller, I2C0);
//     iic_write_addr(caller, I2C0, I2C_SLAVE_BIT7, I2C_TRANSMITTER);//地址 + 写
//     iic_write_byte(caller, I2C0, reg);//写 寄存器
//     iic_stop(caller, I2C0);  //总线结束
//     iic_state_check(caller, caller, I2C0, I2C_FLAG_I2CBSY, 10);
//     iic_start(caller, I2C0);
//     iic_read_addr(caller, I2C0, I2C_SLAVE_BIT7, I2C_RECEIVER);//地址 + 读

//     ret = iic_read_byte(caller, I2C0, 0); //NACK

//     iic_stop(caller, I2C0);  //总线结束

//     return ret;
// }

uint8_t data_to_send[] = {0x33, 0x02, 0x03, 0x04};
uint16_t data_length = sizeof(data_to_send) / sizeof(data_to_send[0]);

// 目标从设备地址
uint8_t slave_address = 0xa0;
uint8_t reg_address = 0x10;

uint8_t rx_buffer[10] = {0};

uint8_t rx1 = 0;
uint8_t rx2 = 0;
uint8_t rx3 = 0;

uint8_t tx_buffer[16] = {0x11, 0x22, 0x33, 0x44}; // 前4元素填充测试

int main(void)
{
  SysModule_Init();
  i2c_nvic_config();

  // i2c0_start();
  // i2c0_send_addr(0xa0);
  // i2c0_send_type(0x22);

  printf(" iic test write \r\n");

  // i2c0_bytes_write(
  //     I2C0,             // I2C外设
  //     slave_address,    // 设备地址
  //     tx_buffer,        // 接收缓冲区
  //     sizeof(tx_buffer) // 读取长度
  // );

  printf(" i2c0 test read data \r\n");
//

uint8_t iic0Send_Data[40]; 
uint8_t * iic0Receive_Data;
 
iic_Init(open_iic0, iic0Send_Data, &iic0Receive_Data, 0xA0); 

  i2c0_master(I2C0, slave_address, reg_address, rx_buffer,sizeof(rx_buffer)); // 读取长度 sizeof(rx_buffer)

  for (int i = 0; i < sizeof(rx_buffer); i++)
  {
    printf("the rx data :0x%02X \r\n", rx_buffer[i]);
  }

  // i2c0_bytes_read(
  //     I2C0,             // I2C外设
  //     slave_address,    // 设备地址
  //     reg_address,      // 寄存器地址
  //     rx_buffer,        // 接收缓冲区
  //     sizeof(rx_buffer) // 读取长度
  // );

  // printf("the rx data :%02X\r\n", rx_buffer[0]);

  // for (int i = 0; i < sizeof(rx_buffer); i++)
  // {

  // printf("the rx data :%02X\r\n", rx_buffer[2]);

  // delay_1ms(100);
  // }

  // ShellCreateTask();
  // TempCreateTask();
  // AdcCreateTask();
  // LedCreateTask();
  // ComCpuCreateTask();
  // NetCreateTask();
  // UdpServerCreateTask();
  // TcpClientCreateTask();
  // vTaskStartScheduler();

  // I2C_Send_Data(0xa0, 0x22);

  printf(" iic test 3 \r\n");
  while (1)
  {
    printf(" iic test 4 \r\n");

    delay_ms(10000);
  };
}
