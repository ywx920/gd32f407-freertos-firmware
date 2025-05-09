#include "delay.h"

#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"	//FreeRtos 系统 头文件
#include "task.h"	  
#endif

#if !SYSTEM_SUPPORT_OS
#define configTICK_RATE_HZ 1000 //裸机使用 滴答定时器 1000HZ
#endif //!SYSTEM_SUPPORT_OS

static uint8_t fac_us=0;	

/*
    滴答定时器初始化
    SYSCLK:系统工作频率
    注意:不管有没有系统支持都需要使用,否则 SysTick_Handler 函数无用
*/
void SYS_INIT(uint16_t SYSCLK)
{
    systick_clksource_set(SYSTICK_CLKSOURCE_HCLK);//选择时钟源

    fac_us=SYSCLK;
    uint32_t reload = SYSCLK;
    reload *= 1000000/configTICK_RATE_HZ;

    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->LOAD =  reload;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

}



//#------------------------------------------------------------------------------------
/*
注意:
    上面的延时不能使用 滴答定时器 延时
    下面的延时才能使用 滴答定时器 延时
*/

__IO uint32_t sysTickCnt;//裸机 中 滴答定时器 计数器
TickFreqTypeDef uwTickFreq = TICK_FREQ_DEFAULT;  /* 1KHz */
#define HAL_MAX_DELAY      0xFFFFFFFFU

/**
  * @brief This function is called to increment  a global variable "uwTick"
  *        used as application time base.
  * @note In the default implementation, this variable is incremented each 1ms
  *       in Systick ISR.
 * @note This function is declared as __weak to be overwritten in case of other
  *      implementations in user file.
  * @retval None
  */
__weak 
void HAL_IncTick(void)
{
    sysTickCnt += (uint32_t)uwTickFreq;
}

/**
  * @brief Provides a tick value in millisecond.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval tick value
  */
__weak 
uint32_t HAL_GetTick(void)
{
    return sysTickCnt;
}

/*
    毫秒 级 延时
    
*/
__weak 
void HAL_Delay(uint32_t Delay)
{
    uint32_t tickstart = HAL_GetTick();
    uint32_t wait = Delay;

    /* Add a freq to guarantee minimum wait */
    if (wait < HAL_MAX_DELAY)
    {
        wait += (uint32_t)(uwTickFreq);
    }

    while ((HAL_GetTick() - tickstart) < wait){}
}



#if SYSTEM_SUPPORT_OS
/* 获取调度计数器 */
uint32_t getSysTickCnt(void)
{
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        return xTaskGetTickCount();
    }
    else
    {
        return HAL_GetTick();
    }

}
#endif //SYSTEM_SUPPORT_OS

void delay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};											    
}  

void delay_ms(uint16_t nms)
{
    delay_us(1000 * nms);
}

