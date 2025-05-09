/*!
    \file    main.c
    \brief   running led

    \version 2024-01-15, V3.2.0, firmware for GD32F4xx
*/

#include "main.h"
// #include "gd32f4xx.h"
#include "systick.h"
#include "sys_config.h"
// #include "bsp_gpio.h"
// #include "bsp_nst175.h"
// #include "commcpu.h"
#include "bsp_adc.h"
// #include "delay.h"
// #include "bsp_uart.h"
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
  nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
  /* led初始化 */
  led_init();
  nst175_init();
  adcinit();
  EEPROM_I2C_CONFIG();
  //		phy_init();
  /* 串口初始化 */
  uart_init(USART0, 115200);
  uart_init(USART1, 115200);
  uart_init(USART2, 115200);

  iic_init(I2C0);
  User_Shell_Init();
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

int main(void)
{
  SysModule_Init();

  masterSend();
  masterRec();

  // ShellCreateTask();
  // TempCreateTask();
  // AdcCreateTask();
  // LedCreateTask();
  // ComCpuCreateTask();
  // NetCreateTask();
  // UdpServerCreateTask();
  // TcpClientCreateTask();
  // vTaskStartScheduler();

  while (1)
  {
    delay_ms(1000);
  };
}
