#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_


/* LED ON/OFF */
#define LED0_ON gpio_bit_reset(GPIOG, GPIO_PIN_0)
#define LED0_OFF gpio_bit_set(GPIOG, GPIO_PIN_0)

#define LED1_ON gpio_bit_reset(GPIOG, GPIO_PIN_1)
#define LED1_OFF gpio_bit_set(GPIOG, GPIO_PIN_1)

#define LED2_ON gpio_bit_reset(GPIOG, GPIO_PIN_2)
#define LED2_OFF gpio_bit_set(GPIOG, GPIO_PIN_2)

#define LED3_ON gpio_bit_reset(GPIOG, GPIO_PIN_3)
#define LED3_OFF gpio_bit_set(GPIOG, GPIO_PIN_3)

#define LED4_ON gpio_bit_reset(GPIOG, GPIO_PIN_4)
#define LED4_OFF gpio_bit_set(GPIOG, GPIO_PIN_4)

#define LED5_ON gpio_bit_reset(GPIOG, GPIO_PIN_5)
#define LED5_OFF gpio_bit_set(GPIOG, GPIO_PIN_5)

#define LED6_ON gpio_bit_reset(GPIOG, GPIO_PIN_6)
#define LED6_OFF gpio_bit_set(GPIOG, GPIO_PIN_6)

#define LED7_ON gpio_bit_reset(GPIOG, GPIO_PIN_7)
#define LED7_OFF gpio_bit_set(GPIOG, GPIO_PIN_7)



void led_init(void);
void ledTASK(void* param);

#endif //_BSP_GPIO_H_
