#include "bsp_phy_LAN8720A.h"
//#include "lwip_config.h"


#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/priv/tcpip_priv.h"
#include "lwip/priv/tcp_priv.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "lwip/sys.h"
#include "lwip/api.h"

///*!
//    \brief      configures the different GPIO ports
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//static void phy_gpio_init(void)
//{
//    /* 引脚时钟打开 */
//    rcu_periph_clock_enable(RCU_GPIOA);
//    rcu_periph_clock_enable(RCU_GPIOC);
//    rcu_periph_clock_enable(RCU_GPIOD);
//    rcu_periph_clock_enable(RCU_GPIOG);
//    
//    /* enable SYSCFG clock */
//    rcu_periph_clock_enable(RCU_AF);
//    
//    /* config RMII_MODE */
//    gpio_ethernet_phy_select(GPIO_ENET_PHY_RMII);
//    
//    /* PHY 引脚映射配置 */
//    gpio_pin_remap1_config(GPIO_PCF5,GPIO_PCF5_ENET_TXD01_REMAP,ENABLE);
//    
//    /* AF Output Push Pull:
//    - ETH_MII_MDIO  / ETH_RMII_MDIO : PA2
//    - ETH_MII_MDC   / ETH_RMII_MDC  : PC1
//    - ETH_MII_TX_EN / ETH_RMII_TX_EN: PG11
//    - ETH_MII_TXD0  / ETH_RMII_TXD0 : PG13
//    - ETH_MII_TXD1  / ETH_RMII_TXD1 : PG14
//    */
//    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
//    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
//    gpio_init(GPIOG, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
//    gpio_init(GPIOG, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
//    gpio_init(GPIOG, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
//    
//    /* Input (Reset Value):
//    - ETH_MII_RX_CLK / ETH_RMII_REF_CLK: PA1
//    - ETH_MII_RX_DV  / ETH_RMII_CRS_DV : PA7
//    - ETH_MII_RXD0   / ETH_RMII_RXD0   : PC4
//    - ETH_MII_RXD1   / ETH_RMII_RXD1   : PC5
//    */
//    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
//    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
//    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
//    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
//    
//    /*
//        PD3:ETH_RESET
//    */
//    gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
//    gpio_bit_reset(GPIOD,GPIO_PIN_3);
//    vTaskDelay(500);
//    gpio_bit_set(GPIOD,GPIO_PIN_3);
//}


///*!
//    \brief      configures the nested vectored interrupt controller
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//static void enet_irq_config(void)
//{
//    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x0);
//    nvic_irq_enable(ENET_IRQn, 2, 0);
//}

///*!
//    \brief      configures the ethernet interface
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//static void mac_dma_config(void)
//{
//    /* enable ethernet clock  */
//    rcu_periph_clock_enable(RCU_ENET);
//    rcu_periph_clock_enable(RCU_ENETTX);
//    rcu_periph_clock_enable(RCU_ENETRX);
//    
//    /* reset ethernet on AHB bus */
//    enet_deinit();
//    
//    if(ERROR == enet_software_reset())
//    {
//        while(1)
//        {
//            vTaskDelay(500);
//            printf("enet_software_reset error\r\n");
//        }
//    }
//    else
//    {
//        printf("enet_software_reset init OK\r\n");
//    }
//}


///*!
//    \brief      configures the ethernet interface
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//static void Enet_init(void)
//{
//    uint32_t enet_init_status = enet_init(ENET_AUTO_NEGOTIATION, ENET_AUTOCHECKSUM_DROP_FAILFRAMES, ENET_BROADCAST_FRAMES_PASS);
//    
//    if(enet_init_status == 0)
//    {
//        while(1)
//        {
//            vTaskDelay(500);
//            printf("enet_init_status error\r\n");
//        }
//    }
//    else
//    {
//        printf("enet_init_status init OK\r\n");
//    }
//    
//    enet_interrupt_enable(ENET_DMA_INT_NIE);
//    enet_interrupt_enable(ENET_DMA_INT_RIE);    
//}





///*
//    获取 PHY 信息
//    适用于 LAN8720A
//*/
//uint32_t get_phy_state(phyState phy)
//{
//    uint32_t phy_value=0xFF;
//    
//    switch((uint8_t)phy)
//    {
//        case PhyIDnumber://ID = 0x300007 否则为错误
//        {
//            uint16_t phy_id_1 = 0;
//            /* 15~0 位有效 ID 的低位 */
//            enet_phy_write_read(ENET_PHY_READ,PHY_ADDRESS,2,&phy_id_1);
//            
//            uint16_t phy_id_2 = 0;
//            /* 15~10 有效 ID 的高位 */
//            enet_phy_write_read(ENET_PHY_READ,PHY_ADDRESS,3,&phy_id_2);
//            phy_value = phy_id_1|((phy_id_2&0xFC00)<<6);
//            phy_id_1=phy_id_2 = 0;
//        }
//        break;
//        
//        case PhyLinkState://0 为 网线丢失; 1 为 网线连接
//        {
//            uint16_t LinkStatus = 0;
//            enet_phy_write_read(ENET_PHY_READ,PHY_ADDRESS,1,&LinkStatus);
//            phy_value = (LinkStatus&0x04)>>2;
//            LinkStatus = 0;
//        }
//        break;
//        
//        case PhyAutoNegotiate://自动协商 0,为自动协商未完成;1,为自动协商完成
//        {
//            uint16_t AutoNegotiate = 0;
//            enet_phy_write_read(ENET_PHY_READ,PHY_ADDRESS,1,&AutoNegotiate);
//            phy_value = (AutoNegotiate&0x20)>>5;
//            AutoNegotiate = 0;
//        }
//        break;
//        
//        default:
//        break;
//    }
//    
//    return phy_value;
//}

//任务处理
//#----------------------------------------------------------------------------
//void NetTask(void *pvParameters)
//{
//    printf("TestTask init OK\r\n");
//    
 #ifdef USE_DHCP
    /* start DHCP client */
//    xTaskCreate(dhcp_task, "DHCP", configMINIMAL_STACK_SIZE * 2, NULL, 5, NULL);
#endif /* USE_DHCP */      
    
    
//    struct netconn *conn, *newconn;
//	err_t err;

//	/* 创建服务器 */
//	conn = netconn_new(NETCONN_TCP);
//	netconn_bind(conn, IP_ADDR_ANY, 8008);

//	/* 设置监听 */
//	netconn_listen(conn);
	
//	while (1) 
//	{
//		/* 等待接收新的连接 */
//		err = netconn_accept(conn, &newconn);

//		/* 处理新连接 */
//		if (err == ERR_OK) 
//		{
//            printf("------------------netconn_accept--------------->\r\n");
//			struct netbuf *buf;
//			void *data;
//			u16_t len;

//			/* 简单的数据回环 */
//			while ((err = netconn_recv(newconn, &buf)) == ERR_OK) 
//			{
//		        do {
//		             netbuf_data(buf, &data, &len);
//		             err = netconn_write(newconn, data, len, NETCONN_COPY);
//		        } while (netbuf_next(buf) >= 0);
//				netbuf_delete(buf);
//			}
//			
//			/* 删除连接 */
//			netconn_close(newconn);
//			netconn_delete(newconn);
//		}

//        vTaskDelay(100);
//	}
//}

//任务处理
//#----------------------------------------------------------------------------
//void LwipTask(void *pvParameters)
//{
//    printf("LwipTask Init ok!\r\n");

//    uint16_t linkStateNum = 0;
//    
//    /* PHY gpio 初始化 */
//    phy_gpio_init();
//    
//    /* PHY 中断配置 */
//    enet_irq_config();
//    
//    /* MAC 配置 */
//    mac_dma_config();
//    
//    /* Enet 网络接口初始化 */
//    Enet_init();
//    
//    lwip_stack_init();
//    
//#ifdef USE_DHCP    
//    if(ThreadTaskId == NULL)
//    {
//        BaseType_t ThreadTaskId = xTaskCreate(dhcp_task, "NetTask", 512, NULL, 4, NULL);
//    }
//#endif /* USE_DHCP */
//    
//    while(1)
//    {
//        if(get_phy_state(PhyLinkState))//判断网线接入
//        {
//            linkStateNum += 1;
//            if(((linkStateNum&0x8000)==0) && (linkStateNum == 20))
//            {
//                linkStateNum = 0x8000;
//                printf("LinkState UP task run\r\n");
//            }
//        }
//        else
//        {
//            linkStateNum = 0;

//            printf("LinkState DOWM\r\n");
//            
//        }
//        
//        vTaskDelay(100);
//    }
//}
