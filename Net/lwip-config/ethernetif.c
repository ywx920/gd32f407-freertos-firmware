/**
  ******************************************************************************
  * File Name          : ethernetif.c
  * Description        : This file provides code for the configuration
  *                      of the ethernetif.c MiddleWare.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "ethernetif.h"
#include <string.h>
//#include "FreeRTOS.h"


/* Within 'USER CODE' section, code will be kept by default at each generation */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Private define ------------------------------------------------------------*/

/* Network interface name */
#define IFNAME0 'L'
#define IFNAME1 'J'


struct ethernetif {
	struct eth_addr *ethaddr;
/* Add whatever per-interface state that is needed here. */
};


xSemaphoreHandle s_xSemaphore = NULL;

sys_sem_t tx_sem = NULL;
sys_mbox_t eth_tx_mb = NULL;

static void arp_timer(void *arg);




/*******************************************************************************
                       LL Driver Interface ( LwIP stack --> ETH) 
*******************************************************************************/
/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{ 
//    HAL_StatusTypeDef hal_eth_init_status;

//    //初始化bsp—eth
//    hal_eth_init_status = Bsp_Eth_Init();

//    if (hal_eth_init_status == HAL_OK)
//    {
//        /* Set netif link flag */  
//        netif->flags |= NETIF_FLAG_LINK_UP;
//    }

#if LWIP_ARP || LWIP_ETHERNET 

    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    /* set MAC hardware address */
    netif->hwaddr[0] =  MAC_ADDR0;
    netif->hwaddr[1] =  MAC_ADDR1;
    netif->hwaddr[2] =  MAC_ADDR2;
    netif->hwaddr[3] =  MAC_ADDR3;
    netif->hwaddr[4] =  MAC_ADDR4;
    netif->hwaddr[5] =  MAC_ADDR5;

    /* maximum transfer unit */
    netif->mtu = NETIF_MTU;

    /* Accept broadcast address and ARP traffic */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
#if LWIP_ARP
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
#else 
    netif->flags |= NETIF_FLAG_BROADCAST;
#endif /* LWIP_ARP */

    /* USER CODE BEGIN PHY_PRE_CONFIG */ 

    s_xSemaphore = xSemaphoreCreateCounting(40,0);

    if(sys_sem_new(&tx_sem , 0) == ERR_OK)
    {
        printf("sys_sem_new ok\r\n");
    }
    

    if(sys_mbox_new(&eth_tx_mb , 50) == ERR_OK)
    {
        printf("sys_mbox_new ok\r\n");
    }

    /* create the task that handles the ETH_MAC */
    sys_thread_new("ETHIN",
                  ethernetif_input,  /* 任务入口函数 */
                  netif,        	  /* 任务入口函数参数 */
                  NETIF_IN_TASK_STACK_SIZE,/* 任务栈大小 */
                  NETIF_IN_TASK_PRIORITY); /* 任务的优先级 */
                  
    //	sys_thread_new("ETHTX",
    //                  ethernetif_output,  /* 任务入口函数 */
    //                  netif,        	  /* 任务入口函数参数 */
    //                  NETIF_OUT_TASK_STACK_SIZE,/* 任务栈大小 */
    //                  NETIF_OUT_TASK_PRIORITY); /* 任务的优先级 */
                                 
    /* USER CODE END PHY_PRE_CONFIG */

#endif /* LWIP_ARP || LWIP_ETHERNET */

//#-----------------------------------------------------------------------
    /* ENET RxDMA/TxDMA descriptor */
    extern enet_descriptors_struct rxdesc_tab[ENET_RXBUF_NUM];
    
    /* initialize MAC address in ethernet MAC */ 
    enet_mac_address_set(ENET_MAC_ADDRESS0, netif->hwaddr);
    /* initialize descriptors list: chain/ring mode */
    enet_descriptors_chain_init(ENET_DMA_TX);
    enet_descriptors_chain_init(ENET_DMA_RX);
    
    /* enable ethernet Rx interrrupt */
    for(int i=0; i<ENET_RXBUF_NUM; i++)
    {
        //enet_desc_receive_complete_bit_enable(&rxdesc_tab[i]);
    }
    
#ifdef CHECKSUM_BY_HARDWARE
    extern enet_descriptors_struct txdesc_tab[ENET_TXBUF_NUM];
    
    /* enable the TCP, UDP and ICMP checksum insertion for the Tx frames */
    for(int i=0; i < ENET_TXBUF_NUM; i++)
    {
        enet_transmit_checksum_config(&txdesc_tab[i], ENET_CHECKSUM_TCPUDPICMP_FULL);
    }
#endif /* CHECKSUM_BY_HARDWARE */
    
    /* enable MAC and DMA transmission and reception */
    enet_enable();
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    extern enet_descriptors_struct  *dma_current_txdesc;
    static xSemaphoreHandle s_tx_semaphore = NULL;
    struct pbuf *q;
    uint8_t *buffer ;
    uint16_t framelength = 0;
    ErrStatus reval = ERROR;
  
    SYS_ARCH_DECL_PROTECT(sr);
    
    if (s_tx_semaphore == NULL)
    {
        vSemaphoreCreateBinary (s_tx_semaphore);//创建二值信号量
    }
    
    if (xSemaphoreTake(s_tx_semaphore, 250))//获取信号量
    {
        SYS_ARCH_PROTECT(sr);
//        buffer = (uint8_t *)(enet_desc_information_get(dma_current_txdesc, TXDESC_BUFFER_1_ADDR));
        buffer = (uint8_t *)dma_current_txdesc->buffer1_addr;

        for(q = p; q != NULL; q = q->next)
        { 
            memcpy((uint8_t *)&buffer[framelength], q->payload, q->len);
            framelength = framelength + q->len;
        }

//        reval = ENET_NOCOPY_FRAME_TRANSMIT(framelength);
        reval = enet_frame_transmit(buffer,framelength);

        SYS_ARCH_UNPROTECT(sr);
        
        /* give semaphore and exit */
        xSemaphoreGive(s_tx_semaphore);//释放信号量
    }
    
    if(SUCCESS == reval)
    {
        return ERR_OK;
    }
    else
    {
        return ERROR;
    }
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
   */
static struct pbuf * low_level_input(struct netif *netif)
{
    extern enet_descriptors_struct  *dma_current_rxdesc;
    
    struct pbuf *p= NULL, *q= NULL;
    uint32_t l =0;
    u16_t len;
    uint8_t *buffer;
  
    /* obtain the size of the packet and put it into the "len" variable. */
    len = enet_desc_information_get(dma_current_rxdesc, RXDESC_FRAME_LENGTH);
    buffer = (uint8_t *)(enet_desc_information_get(dma_current_rxdesc, RXDESC_BUFFER_1_ADDR));
  
//    printf("receive frame len : %d\r\n", len);
  
    if (len > 0)
    {
        /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
        p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
        
        if (p != NULL)
        {
            for(q = p; q != NULL; q = q->next)
            {
                memcpy((uint8_t *)q->payload, (u8_t*)&buffer[l], q->len);
                l = l + q->len;
            }
        }
    }
  
    ENET_NOCOPY_FRAME_RECEIVE();

    return p;
}


//static err_t ethernetif_linkoutput(struct netif *netif, struct pbuf *p)
//{
//  netif = NULL;
//  void *msg;
//  
//  msg = p;
//  
//  if(sys_mbox_trypost(&eth_tx_mb,&msg) == ERR_OK)
//  {
//    PRINT_DEBUG("sys_mbox_trypost ok\n");
////    xSemaphoreTake(tx_sem,portMAX_DELAY);
//  }
//  
//  
//  return ERR_OK; 
//}

//void ethernetif_output(void *pParams) 
//{
//  void *msg;
//  struct pbuf *p;
//  struct netif *netif;  
//  netif = (struct netif*) pParams;
//  while(1)
//  {
//    if(sys_arch_mbox_fetch(&eth_tx_mb,&msg,0)==ERR_OK)
//    {
//      PRINT_DEBUG("sys_arch_mbox_fetch ok\n");
//      p = (struct pbuf *)msg;
//      
//      if(p!=NULL)
//      {
//        if(low_level_output(netif,p) == ERR_OK)
//        {
//          PRINT_DEBUG("low_level_output ok\n");
//          LED3_TOGGLE;
//        }
//      }
//      
//      /* send ACK */
////      sys_sem_signal(&tx_sem);
//    }
////    else
////      PRINT_DEBUG("not ok\n");
//  }
//  
//}


/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
//void ethernetif_input(void *pParams) {
//	struct netif *netif;
//	struct pbuf *p = NULL;
//	netif = (struct netif*) pParams;
//  LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
//  
//	while(1) 
//  {
//    if(xSemaphoreTake( s_xSemaphore, portMAX_DELAY ) == pdTRUE)
//    {
//      /* move received packet into a new pbuf */
//      taskENTER_CRITICAL();
//      p = low_level_input(netif);
//      taskEXIT_CRITICAL();
//      /* points to packet payload, which starts with an Ethernet header */
//      if(p != NULL)
//      {
//        taskENTER_CRITICAL();
//        /* full packet send to tcpip_thread to process */
//        if (netif->input(p, netif) != ERR_OK)
//        {
//          LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
//          pbuf_free(p);
//          p = NULL;
//        }
//        taskEXIT_CRITICAL();
//      }
//    }
//	}
//}
void ethernetif_input(void *pParams) 
{
    struct netif *netif;
    struct pbuf *p = NULL;
    netif = (struct netif*) pParams;

    while(1) 
    {
        if(xSemaphoreTake( s_xSemaphore, portMAX_DELAY ) == pdTRUE)
        {
            /* move received packet into a new pbuf */
            taskENTER_CRITICAL();
            
            TRY_GET_NEXT_FRAGMENT:
            
            p = low_level_input(netif);
            
            taskEXIT_CRITICAL();
            
            /* points to packet payload, which starts with an Ethernet header */
            if(p != NULL)
            {
                taskENTER_CRITICAL();
                
                /* full packet send to tcpip_thread to process */
                if (netif->input(p, netif) != ERR_OK)
                {
                    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input 456: IP input error\n"));
                    pbuf_free(p);
                    p = NULL;
                }
                else
                {
                    xSemaphoreTake( s_xSemaphore, 0);
                    goto TRY_GET_NEXT_FRAGMENT;
                }
                
                taskEXIT_CRITICAL();
            }
        }
    }
}

#if !LWIP_ARP
/**
 * This function has to be completed by user in case of ARP OFF.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if ...
 */
static err_t low_level_output_arp_off(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{  
  err_t errval;
  errval = ERR_OK;
    
/* USER CODE BEGIN 5 */ 
    
/* USER CODE END 5 */  
    
  return errval;
  
}
#endif /* LWIP_ARP */ 

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
    struct ethernetif *ethernetif;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    ethernetif = mem_malloc(sizeof(struct ethernetif));

    if (ethernetif == NULL)
    {
        printf("ethernetif_init: out of memory \r\n");
        return ERR_MEM;
    }

    LWIP_ASSERT("netif != NULL", (netif != NULL));
    
#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */
    
    netif->state = ethernetif;
    
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    
    /* We directly use etharp_output() here to save a function call.
    * You can instead declare your own function an call etharp_output()
    * from it if you have to do some checks before sending (e.g. if link
    * is available...) */

#if LWIP_IPV4
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
    netif->output = etharp_output;
#else
    /* The user should write ist own code in low_level_output_arp_off function */
    netif->output = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */

    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);
    ethernetif->ethaddr = (struct eth_addr *) &(netif->hwaddr[0]);
    
    //  etharp_init();
    //  sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
    
    return ERR_OK;
}

static void arp_timer(void *arg)
{
  etharp_tmr();
  sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
}

/* USER CODE BEGIN 6 */

#if LWIP_NETIF_LINK_CALLBACK
/**
  * @brief  Link callback function, this function is called on change of link status
  *         to update low level driver configuration.
* @param  netif: The network interface
  * @retval None
  */
void ethernetif_update_config(struct netif *netif)
{
  __IO uint32_t tickstart = 0;
  uint32_t regvalue = 0;
  
  if(netif_is_link_up(netif))
  { 
    /* Restart the auto-negotiation */
    if(heth.Init.AutoNegotiation != ETH_AUTONEGOTIATION_DISABLE)
    {
      /* Enable Auto-Negotiation */
      HAL_ETH_WritePHYRegister(&heth, PHY_BCR, PHY_AUTONEGOTIATION);
      
      /* Get tick */
      tickstart = HAL_GetTick();
      
      /* Wait until the auto-negotiation will be completed */
      do
      {
        HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &regvalue);
        
        /* Check for the Timeout ( 1s ) */
        if((HAL_GetTick() - tickstart ) > 1000)
        {     
          /* In case of timeout */ 
          goto error;
        }   
      } while (((regvalue & PHY_AUTONEGO_COMPLETE) != PHY_AUTONEGO_COMPLETE));
      
      /* Read the result of the auto-negotiation */
      HAL_ETH_ReadPHYRegister(&heth, PHY_SR, &regvalue);
      
      /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
      if((regvalue & PHY_DUPLEX_STATUS) != (uint32_t)RESET)
      {
        /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
        heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;  
      }
      else
      {
        /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
        heth.Init.DuplexMode = ETH_MODE_HALFDUPLEX;           
      }
      /* Configure the MAC with the speed fixed by the auto-negotiation process */
      if(regvalue & PHY_SPEED_STATUS)
      {  
        /* Set Ethernet speed to 10M following the auto-negotiation */
        heth.Init.Speed = ETH_SPEED_10M; 
      }
      else
      {   
        /* Set Ethernet speed to 100M following the auto-negotiation */ 
        heth.Init.Speed = ETH_SPEED_100M;
      }
    }
    else /* AutoNegotiation Disable */
    {
    error :
      /* Check parameters */
      assert_param(IS_ETH_SPEED(heth.Init.Speed));
      assert_param(IS_ETH_DUPLEX_MODE(heth.Init.DuplexMode));
      
      /* Set MAC Speed and Duplex Mode to PHY */
      HAL_ETH_WritePHYRegister(&heth, PHY_BCR, ((uint16_t)(heth.Init.DuplexMode >> 3) |
                                                     (uint16_t)(heth.Init.Speed >> 1))); 
    }

    /* ETHERNET MAC Re-Configuration */
    HAL_ETH_ConfigMAC(&heth, (ETH_MACInitTypeDef *) NULL);

    /* Restart MAC interface */
    HAL_ETH_Start(&heth);   
  }
  else
  {
    /* Stop MAC interface */
    HAL_ETH_Stop(&heth);
  }

  ethernetif_notify_conn_changed(netif);
}

/* USER CODE BEGIN 8 */
/**
  * @brief  This function notify user about link status changement.
  * @param  netif: the network interface
  * @retval None
  */
__weak void ethernetif_notify_conn_changed(struct netif *netif)
{
  /* NOTE : This is function could be implemented in user file 
            when the callback is needed,
  */

}
/* USER CODE END 8 */ 
#endif /* LWIP_NETIF_LINK_CALLBACK */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

