#ifndef _BSP_PHY_LAN8720A_H_
#define _BSP_PHY_LAN8720A_H_

#include "main.h"


typedef enum _phyState{
    PhyIDnumber = 1,    /* PHY id 编号 */
    PhyLinkState,       /* PHY 网络状态 */
    PhyAutoNegotiate,   /* PHY 自动协商状态 */
}const phyState;


void LwipTask(void *pvParameters);

#endif //_BSP_PHY_LAN8720A_H_
