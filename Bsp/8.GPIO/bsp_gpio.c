#include "bsp_gpio.h"

#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"
#include "task.h"
#endif //SYSTEM_SUPPORT_OS

#include "shell_port.h"
#include "main.h"

/*****
        LED0 D80    ---> PG0
        LED1 D81    ---> PG1
        LED2 D82    ---> PG2
        LED3 D83    ---> PG3
        LED4 D84    ---> PG4
        LED5 D85    ---> PG5
				LED6 D86    ---> PG6
				LED7 D87    ---> PG7
*****/
void led_init(void)
{
		/***时钟初始化***/	
		rcu_periph_clock_enable(RCU_GPIOG);
	
		/***引脚模式初始化***/
		gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0 | \
																GPIO_PIN_1 | \
																GPIO_PIN_2 | \
																GPIO_PIN_3 | \
																GPIO_PIN_4 | \
																GPIO_PIN_5 | \
																GPIO_PIN_6 | \
																GPIO_PIN_7 );
		gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0| GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3| GPIO_PIN_4| GPIO_PIN_5| GPIO_PIN_6| GPIO_PIN_7);
		/***引脚默认状态：高电平--熄灭***/
		gpio_bit_set(GPIOG, GPIO_PIN_0);  
		gpio_bit_set(GPIOG, GPIO_PIN_1); 
		gpio_bit_set(GPIOG, GPIO_PIN_2); 
		gpio_bit_set(GPIOG, GPIO_PIN_3); 
		gpio_bit_set(GPIOG, GPIO_PIN_4); 
		gpio_bit_set(GPIOG, GPIO_PIN_5);
		gpio_bit_set(GPIOG, GPIO_PIN_6); 
		gpio_bit_set(GPIOG, GPIO_PIN_7);
}
void ledTASK(void* param)
{
	while(1)
	{
			vTaskDelay(200);
			LED0_ON;
			LED1_ON;
			LED2_ON;
			LED3_ON;
			LED4_ON;
			LED5_ON;
			LED6_ON;
			LED7_ON;
			vTaskDelay(200);
			LED0_OFF;
			LED1_OFF;
			LED2_OFF;
			LED3_OFF;
			LED4_OFF;
			LED5_OFF;
			LED6_OFF;
			LED7_OFF;
	}

}

void led_test(void)
{
			delay_ms(1000);
			LED0_ON;
			LED1_ON;
			LED2_ON;
			LED3_ON;
			LED4_ON;
			LED5_ON;
			LED6_ON;
			LED7_ON;
			printf("\r\n led on");
			delay_ms(1000);
			LED0_OFF;
			LED1_OFF;
			LED2_OFF;
			LED3_OFF;
			LED4_OFF;
			LED5_OFF;
			LED6_OFF;
			LED7_OFF;
			printf("\r\n led off");
			delay_ms(1000);
			LED0_ON;
			LED1_ON;
			LED2_ON;
			LED3_ON;
			LED4_ON;
			LED5_ON;
			LED6_ON;
			LED7_ON;
			printf("\r\n led on");
			delay_ms(1000);
			LED0_OFF;
			LED1_OFF;
			LED2_OFF;
			LED3_OFF;
			LED4_OFF;
			LED5_OFF;
			LED6_OFF;
			LED7_OFF;
			printf("\r\n led off");

}
	

SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
LED_TEST, led_test, led test);










