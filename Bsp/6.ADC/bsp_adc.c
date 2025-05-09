#include "bsp_adc.h"
#include <stdio.h>
// #include "bsp_gpio.h"
// #include "delay.h"
#include "shell_port.h"
static uint32_t adc_value[5];

void adc_gpio_init(void)
{
	/* 使能 ADC 时钟 */
	rcu_periph_clock_enable(RCU_ADC2);
	// /* 使能 GPIO 时钟 */
	// rcu_periph_clock_enable(RCU_GPIOF);
	/* enable timer1 clock */
	rcu_periph_clock_enable(RCU_TIMER1);
	rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
	/* 配置 ADC 时钟 */
	adc_clock_config(ADC_ADCCK_PCLK2_DIV4);
	/***引脚模式初始化***/
	gpio_mode_set(GPIOF, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);
}

void adc_dma_config(void)
{
	/* ADC_DMA_channel configuration */
	dma_single_data_parameter_struct dma_single_data_parameter;
	/* 使能 DMA1  */
	rcu_periph_clock_enable(RCU_DMA1);
	/* ADC DMA_channel configuration */
	dma_deinit(DMA1, DMA_CH1);
	/* initialize DMA single data mode */
	dma_single_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC2));
	dma_single_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_single_data_parameter.memory0_addr = (uint32_t)(&adc_value);
	dma_single_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_32BIT;
	dma_single_data_parameter.direction = DMA_PERIPH_TO_MEMORY;
	dma_single_data_parameter.number = 5;
	dma_single_data_parameter.priority = DMA_PRIORITY_HIGH;
	dma_single_data_mode_init(DMA1, DMA_CH1, &dma_single_data_parameter);
	dma_channel_subperipheral_select(DMA1, DMA_CH1, DMA_SUBPERI2);
	/* enable DMA circulation mode */
	dma_circulation_enable(DMA1, DMA_CH1);
	/* enable DMA channel */
	dma_channel_enable(DMA1, DMA_CH1);
}

void adc_config(void)
{
	adc_deinit();
	/* ADC mode config */
	adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
	/* ADC contineous function disable */
	adc_special_function_config(ADC2, ADC_CONTINUOUS_MODE, ENABLE);
	/* ADC scan mode disable */
	adc_special_function_config(ADC2, ADC_SCAN_MODE, ENABLE);
	/* ADC data alignment config */
	adc_data_alignment_config(ADC2, ADC_DATAALIGN_RIGHT);

	/* ADC channel length config */
	adc_channel_length_config(ADC2, ADC_ROUTINE_CHANNEL, 5);
	/* ADC routine channel config */
	adc_routine_channel_config(ADC2, 0, ADC_CHANNEL_4, ADC_SAMPLETIME_15);
	adc_routine_channel_config(ADC2, 1, ADC_CHANNEL_5, ADC_SAMPLETIME_15);
	adc_routine_channel_config(ADC2, 2, ADC_CHANNEL_6, ADC_SAMPLETIME_15);
	adc_routine_channel_config(ADC2, 3, ADC_CHANNEL_7, ADC_SAMPLETIME_15);
	adc_routine_channel_config(ADC2, 4, ADC_CHANNEL_8, ADC_SAMPLETIME_15);
	/* ADC trigger config */
	adc_external_trigger_source_config(ADC2, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T1_CH2);
	adc_external_trigger_config(ADC2, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

	/* ADC DMA function enable */
	adc_dma_request_after_last_enable(ADC2);
	adc_dma_mode_enable(ADC2);

	/* enable ADC interface */
	adc_enable(ADC2);
	/* wait for ADC stability */
	delay_ms(1);
	/* ADC calibration and reset calibration */
	adc_calibration_enable(ADC2);

	/* enable ADC software trigger */
	adc_software_trigger_enable(ADC2, ADC_ROUTINE_CHANNEL);
}

float adc4_value = 0;
float adc5_value = 0;
float adc6_value = 0;
float adc7_value = 0;
float adc8_value = 0;

int getAdc0(void) { return adc4_value * 1000; }
int getAdc1(void) { return adc5_value * 1000; }
int getAdc2(void) { return adc6_value * 1000; }
int getAdc3(void) { return adc7_value * 1000; }
int getAdc4(void) { return adc8_value * 1000; }

void adcinit()
{
	adc_gpio_init();
	adc_config();
	adc_dma_config();
}
void adcTask(void *param)
{

	// adc_gpio_init();
	// adc_dma_config();
	// adc_config();

	/* https://blog.csdn.net/xxdx_admin/article/details/114284458 */

	while (1)
	{

		/* enable ADC software trigger */
		adc_software_trigger_enable(ADC2, ADC_ROUTINE_CHANNEL);
		vTaskDelay(300);
		adc4_value = (float)(adc_value[0] * 3.3 / 4096); // IN1
		adc5_value = (float)(adc_value[1] * 3.3 / 4096); // IN2
		adc6_value = (float)(adc_value[2] * 3.3 / 4096); // IN3
		adc7_value = (float)(adc_value[3] * 3.3 / 4096); // IN4
		adc8_value = (float)(adc_value[4] * 3.3 / 4096); // IN5
	}
}

void adcprintf()
{
	adc4_value = (float)(adc_value[0] * 3.3 / 4096); // IN1
	adc5_value = (float)(adc_value[1] * 3.3 / 4096); // IN2
	adc6_value = (float)(adc_value[2] * 3.3 / 4096); // IN3
	adc7_value = (float)(adc_value[3] * 3.3 / 4096); // IN4
	adc8_value = (float)(adc_value[4] * 3.3 / 4096); // IN5
	printf("\r\n adc4_value:%.2fv(12V)\r\n adc5_value:%.2fv(5V)\r\n adc6_value:%.2fv\r\n adc7_value:%.2fv\r\n adc8_value:%.2fv\r\n",
		   adc4_value, adc5_value, adc6_value, adc7_value, adc8_value);
}
SHELL_EXPORT_CMD(
	SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
	adcprintf, adcprintf, printf adc vaule);
