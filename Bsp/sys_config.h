#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H
#include <FreeRTOS.h>
#include <task.h>
#include "0.SYSTICK/delay.h"
#include "0.SYSTICK/mmalloc.h"
#include "0.SYSTICK/systick.h"
#include "3.USART/bsp_uart.h"
#include "5.ENET/bsp_phy_LAN8720A.h"
#include "5.ENET/bsp_phy_YT8512H.h"
#include "6.ADC/bsp_adc.h"
#include "7.IIC/bsp_i2c.h"
#include "7.IIC/bsp_iicBus.h"
#include "8.GPIO/bsp_gpio.h"
#include "8.GPIO/bsp_gx18b20.h"
#include "9.9056/bsp_Control_9056.h"
#include "10.2020/bps_Control_2020.h"
#include "11.XC6S/bsp_Control_xc6s.h"
#include "12.TMP100/bsp_tmp100.h"
#include "13.NCT210/bsp_nct210.h"
#include "14.IPMB/ipmb.h"
#include "15.myString/myString.h"
// #include "16BitDOS/common/portasm.h"
#include "17.RS485/rs485.h"
#include "18.COMCPU/commcpu.h"
#include "19.NST175/bsp_nst175.h"
#include "20.Simulate_IIC/simulate_iic.h"

#include "22.EEPROM/i2c_gpio.h"
#include "22.EEPROM/i2c_ee.h"



/* 任务模块 */
// TaskHandle_t Shell_task_Handle;
// TaskHandle_t Temp_Task_Handle;
// TaskHandle_t Adc_Task_Handle;
// TaskHandle_t Led_Task_Handle; 
// TaskHandle_t Cpuinfo_Task_Handle;


/* flash分区划分 */
#define FLASH_BOOT_ADDR             (0x00)
#define FLASH_APP1_ADDR             (0x9000)
#define FLASH_APP2_ADDR             (0xA9000)
#define FLASH_PARAM_ADDR            (0x149000)
#define FLASH_FONT_ADDR             (0x151000)
#define FLASH_GBK14_ADDR             (0x151000)
#define FLASH_GBK12_ADDR             (0x191000)
#define FLASH_ASCLL_ADDR             (0x1C1000)
#define FLASH_SOUND_ADDR            (0x1D0000)

#define MI_RECORD_SIZE                  0x1000
#define FLASH_ADR_MI_RECORD_A_START		0x1DB000
#define FLASH_ADR_MI_RECORD_A_END		(FLASH_ADR_MI_RECORD_A_START+MI_RECORD_SIZE)
#define FLASH_ADR_MI_RECORD_B_START		0x1DC000
#define FLASH_ADR_MI_RECORD_B_END		(FLASH_ADR_MI_RECORD_B_START+MI_RECORD_SIZE)

#define FLASH_USER_TWS_BASE        	0x1E0000     // bt_ram_vari_para
#define FLASH_SYS_USER_BASE         0x1E1000     //+0xEC000  UAER DATA , PAIRING INFO ...
#define FLASH_USER_EQ_BASE			0x1E2000		//music EQ and speech EQ para
#define FLASH_USER_NAME_BASE        0x1E3000     // NAME
#define FLASH_USER_CONST_BASE       0x1E4000     // ADDR

#define BOOT_MAX_SIZE                (0x8000)
#define APP_MAX_SIZE                 (0xA0000)
#define FONT_MAX_SIZE                (0x7F000)
#define GBK14_MAX_SIZE                (0x40000)
#define GBK12_MAX_SIZE                (0x30000)
#define ASCLL_MAX_SIZE                (0xF000)
#define SOUND_MAX_SIZE                (0xB000)
//系统调度管理
#define TASK_SCHEDULE_LOCK    vTaskSuspendAll()
#define TASK_SCHEDULE_UNLOCK  xTaskResumeAll()
#define STORE_HEAP_SUPPORT

#define ACK 0
#define NAK 1

#define RET_OK  (0)
#define RET_ERR (1)

#define BOOT_INFO_ADDR 0x8200
#define BOOT_DEF_STATE 0xFFFFFFFF   //默认状态boot中直接跳转，APP中不需要处理
#define BOOT_WAIT_COPY 0x11221122   //升级完成等待拷贝状态
#define BOOT_WAIT_REBOOT 0x11331133 //需要重启状态
#define BOOT_ENTER_BOOT 0x11441144  //进入boot状态

#define FLASH_SECTOR_SIZE (0x1000)
#define FLASH_PAGE_SIZE (0x100)

typedef enum
{
    PATH_STATE_OFF = 0,
    PATH_STATE_ON = 1,
}PATH_STATE;

#endif