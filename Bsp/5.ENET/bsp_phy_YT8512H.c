#include "bsp_phy_YT8512H.h"
#include "sys_arch.h"
#include "lwip_client.h"



/*
MCU_RMII_REF_CLK          TXC			PA1
MCU_RMII_TX_EN						TXEN		PG11
MCU_RMII_TXD0							TXD0		PG13
MCU_RMII_TXD1							TXD1		PG14
ETH_MDIO 									MDIO		PA2  
ETH_MDC										MDC			PC1


MCU_RMII_RXD0							RXD0		PC4
MCU_RMII_RXD1							RXD1		PC5
MCU_RMII_INT_N										PG15
MCU_RMII_CRS_DV										PA7
ETH_RESET               					PD3
*/


void phy_gpio_init(void)
{
    /* 引脚时钟打开 */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
		rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOG);    
    
    /* enable SYSCFG clock */
    rcu_periph_clock_enable(RCU_SYSCFG);
    
    /* config RMII_MODE */
    syscfg_enet_phy_interface_config(SYSCFG_ENET_PHY_RMII);
    
    /* PA1: ETH_RMII_REF_CLK */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_1);

    /* PA2: ETH_MDIO */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_2);

    /* PA7: ETH_RMII_CRS_DV */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_7);

		gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_1);
	  gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_2);
		gpio_af_set(GPIOA, GPIO_AF_11, GPIO_PIN_7);		
		
    /* PG11: ETH_RMII_TX_EN */
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_11);

    /* PG13: ETH_RMII_TXD0 */
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_13);

    /* PG14: ETH_RMII_TXD1 */
    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_14);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_14);
		
		gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_11);
		gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_13);
		gpio_af_set(GPIOG, GPIO_AF_11, GPIO_PIN_14);		
		
    /* PC1: ETH_MDC */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_1);

    /* PC4: ETH_RMII_RXD0 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_4);

    /* PC5: ETH_RMII_RXD1 */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_5);

		gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_1);
		gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_4);		
		gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_5);		
		
		/* PD3: ETH_RESET */
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
		
		/* PHY RESET */
    gpio_bit_reset(GPIOD, GPIO_PIN_3);
		vTaskDelay(50);
		gpio_bit_set(GPIOD, GPIO_PIN_3);

}

void phy_enet_irq(void)
{
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x0);
		//nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(ENET_IRQn, 5, 0);
}

void phy_dma_init(void)
{
    /* enable ethernet clock  */
    rcu_periph_clock_enable(RCU_ENET);
    rcu_periph_clock_enable(RCU_ENETTX);
    rcu_periph_clock_enable(RCU_ENETRX);
    
    /* reset ethernet on AHB bus */
    enet_deinit();
    vTaskDelay(100);
    if(ERROR == enet_software_reset())
    {
        while(1)
        {
            vTaskDelay(50);
            printf("enet_software_reset error\r\n");
        }
    }
    else
    {
        printf("enet_software_reset OK\r\n");
    }
}

static void phy_enet_init(void)
{
    int timeout = 0;
    while (enet_init(ENET_100M_FULLDUPLEX, ENET_AUTOCHECKSUM_DROP_FAILFRAMES, ENET_BROADCAST_FRAMES_PASS) == 0)
    {
        printf("loop enetinit\r\n");
        vTaskDelay(50);
        timeout++;
		#if 1
        if (timeout > 1)
        {
            printf("loop enetinit erro\r\n");
            break;
        }
		#endif
    }

    enet_interrupt_enable(ENET_DMA_INT_NIE);
    enet_interrupt_enable(ENET_DMA_INT_RIE);
}

void phy_init(void)
{
    /* 网络初始化 */
    phy_gpio_init();
    phy_enet_irq();
    phy_dma_init();
    
    phy_enet_init();
    
    TCPIP_Init();//网络协议栈
}
extern void thread_server_entry(void *param);
extern void netif_link_stat(void *param);

//任务处理
//#----------------------------------------------------------------------------

void netTask(void *pvParameters)
{
    printf("netTask run!~~~~~\r\n");

    phy_init();//网络初始化
    printf("phy init over\r\n");

#if LWIP_DHCP
    sys_thread_new("lwip_dhcp", lwipclient, NULL, 512, 5);
#endif /* LWIP_DHCP */

    while (1)
    {

        vTaskDelay(300);
    }
}

void phy_statu()
{
    uint16_t phy_value_yt = 0;
    enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, 1, &phy_value_yt);
    printf("phy statu:0x%X", phy_value_yt);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    phystatu, phy_statu, read phy statu regist);

void phy_contol()
{
	uint16_t phy_value_yt = 0;
	enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, 0, &phy_value_yt);
	printf("phy contol:0x%X", phy_value_yt);
}

SHELL_EXPORT_CMD(
	SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
	phycontol, phy_contol, read phy contol regist);

void phy_reset()
{
	gpio_bit_reset(GPIOD, GPIO_PIN_9);
  delay_us(5000);
	delay_us(5000);
	delay_us(5000);
	delay_us(5000);
	delay_us(5000);
	delay_us(5000);
	delay_us(5000);
	delay_us(5000);
	delay_us(5000);
	delay_us(5000);
    gpio_bit_set(GPIOD, GPIO_PIN_9);
	printf("reset over\r\n");
}
SHELL_EXPORT_CMD(
	SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
	phyreset, phy_reset, reset phy);



