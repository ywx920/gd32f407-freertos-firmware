#ifndef _BSP_PHY_YT8512H_H_
#define _BSP_PHY_YT8512H_H_

#include "main.h"

void phy_gpio_init(void);
void phy_enet_irq(void);
void phy_dma_init(void);
void netTask(void *pvParameters);
void phy_init(void);
#endif //_BSP_PHY_YT8512H_H_
