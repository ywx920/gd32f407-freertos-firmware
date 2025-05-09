/**************************************************************
 * 版权    : ZONKIN @2022
 * 文件名  : ipmb.c
 * 文件说明: ipmb配置文件
 * 创建人  : lht 2022-9-01
 * 修改时间: 
 * ***********************************************************/

#include "ipmb.h"
#include "bsp_i2c.h"
// #include "moni_iic.h"
#include "bsp_uart.h"
#include "bsp_gpio.h"
// #include "bsp_Control_9056.h"

#define IIC_TEST_MODE 0  /* 0: lib; 1: moni; 2: slave */
/*
请求报文
+------+-----------------------------------------------------------+
|      |                          比特位                           |
| 字节 +-----+-----+-----+-----+-----+-----+-----------+-----------+
|      |  7  |  6  |  5  |  4  |  3  |  2  |       1   |       0   |
+------------+-----+-----+-----+-----+-----+-----------+-----------+
|  1   |          响应从地址(rsSA)                                 |
+------------------------------------------+-----------------------+
|  2   |          网络功能码(netFn)		   | 响应逻辑单元号(rsLUN) |
+------------------------------------------+-----------------------+
|  3   |          校验码1(Checksum)                                |
+------------------------------------------------------------------+
|  4   |          请求从地址(rqSA)								   |
+------------------------------------------+-----------------------+
|  5   |          请求序列号(rqSeq)        | 请求逻辑单元号(rqLUN) |
+------------------------------------------+-----------------------+
|  6   |          命令号(Command)								   |
+------------------------------------------------------------------+
| 7:N  |          数据(Data)                                       |
+------------------------------------------------------------------+
| N+1  |          校验码2(Checksum)                                |
+------+-----------------------------------------------------------+
*/

/*
响应报文
+------+-----------------------------------------------------------+
|      |                          比特位                           |
| 字节 +-----+-----+-----+-----+-----+-----+-----------+-----------+
|      |  7  |  6  |  5  |  4  |  3  |  2  |       1   |       0   |
+------------+-----+-----+-----+-----+-----+-----------+-----------+
|  1   |          请求从地址(rqSA)                                 |
+------------------------------------------+-----------------------+
|  2   |          网络功能码(netFn)		   | 请求逻辑单元号(rqLUN) |
+------------------------------------------+-----------------------+
|  3   |          校验码1(Checksum)                                |
+------------------------------------------------------------------+
|  4   |          响应从地址(rsSA)								   |
+------------------------------------------+-----------------------+
|  5   |          请求序列号(rqSeq)        | 响应逻辑单元号(rsLUN) |
+------------------------------------------+-----------------------+
|  6   |          命令号(Command)								   |
+------------------------------------------------------------------+
|  7   |          完成码(Completion Code)                          |
+------------------------------------------------------------------+
| 8:N  |          数据(Data)                                       |
+------------------------------------------------------------------+
| N+1  |          校验码2(Checksum)                                |
+------+-----------------------------------------------------------+
*/

//请求
typedef struct _ipmb_rq_msghd {
    uint8_t rsSA;    //响应地址(目标地址)
    uint8_t netFn:6; //网络功能码
    uint8_t rsLUN:2; //响应逻辑单元号
    uint8_t rqSA;    //请求地址(源地址)
    uint8_t rqSeq:6; //请求序列号
    uint8_t rqLUN:2; //请求逻辑单元号
    uint8_t cmd;     //命令码
}ipmb_rq_msghd;


//响应
typedef struct _ipmb_rs_msghd{
    uint8_t rqSA;   //请求地址(目标地址)
    uint8_t netFn;  //网络功能码
    uint8_t rqLUN;  //请求逻辑单元号
    uint8_t rsSA;   //响应地址(源地址)
    uint8_t rqSeq;  //请求序列号
    uint8_t rsLUN;  //响应逻辑单元号
    uint8_t cmd;    //命令码
}ipmb_rs_msghd;


 /**********************************************************************
 *1-函数名称:ipmb_send
 *2-函数功能:IPMB 发送
 *3-输入参数:   i2c_periph,表示IPMI接口编号(I2C0,I2C1,I2C2)
 *              destaddr,表示目的地址
 *              byte_num,表示发送信息字节长度
 *              ipmb_sendmsg,发送数据缓冲区
 *4-返回值:无
 **********************************************************************/
uint8_t ipmb_send(uint32_t i2c_periph, uint16_t destaddr, uint8_t byte_num, uint8_t* ipmb_sendmsg)
{
    uint16_t  timeout = 500;
	char caller[11] = "ipmb_send";
	iic_state ret = IIC_FLAG_OK;
	uint8_t addr_cvt = (destaddr) & 0xff;
    
    /* 检查 I2C 总线 空闲 */
	caller[9] = '1';
	ret = iic_state_check(caller, caller, i2c_periph, I2C_FLAG_I2CBSY, timeout);
    if( ret ) return ret;
    /* 启动 I2C 总线 */
	caller[9] = '2';
    ret = iic_start(caller, i2c_periph);
    if( ret ) return ret;
	// vTaskDelay(100);
    /* 发送 I2C 总线 设备地址 */
	caller[9] = '3';
	ret = iic_write_addr(caller, i2c_periph, addr_cvt, I2C_TRANSMITTER);
    if( ret ) return ret;
    /* 开始传输数据 */
    for(uint8_t i=0; i < byte_num; i++)
    {
        i2c_data_transmit(i2c_periph,ipmb_sendmsg[i]);
		caller[10] = '4' + i;
		ret = iic_state_check(caller, caller, i2c_periph, I2C_FLAG_BTC, timeout);

		if( ret ) return ret;
    }
	// vTaskDelay(50);
    /* I2C 总线 停止 */
	caller[9] = '0';
    ret = iic_stop(caller, i2c_periph);

    return ret;
}

/**********************************************************************
 *1-函数名称:ipmb_checksum
 *2-函数功能:IPMB 校验
 *3-输入参数:
 *           data,IPMB 命令消息
 *           size,命令消息长度
 *4-返回值:
 **********************************************************************/
static uint8_t ipmb_checksum(const unsigned char *data, int size)
{
	unsigned char csum = 0;

	for (; size > 0; size--, data++)
    {
        csum += *data;
    }

	return -csum;
}

/**********************************************************************
 *1-函数名称: format_ipmb_rsp
 *2-函数功能:IPMB 请求报文(封包)
 *3-输入参数:
 *           data,IPMB 命令消息
 *           size,命令消息长度
 *4-返回值:
 **********************************************************************/
static uint32_t format_ipmb_msg(uint8_t *msg, const uint8_t *omsg,uint32_t omsg_len,ipmb_rq_msghd *mi)
{
    unsigned int msg_len;
    
    msg[0] = mi->rsSA;                      //响应地址
    msg[1] = (mi->netFn << 2) | mi->rsLUN;  //功能码 | 响应逻辑单元号
    msg[2] = ipmb_checksum(msg, 2);         //验证码1
    msg[3] = mi->rqSA;                      //请求从地址
    msg[4] = (mi->rqSeq << 2) | mi->rqLUN;  //请求序列号 | 请求逻辑单元号
    msg[5] = mi->cmd;                       //命令号
    memcpy(msg+6, omsg, omsg_len);          //数据
    msg_len = omsg_len + 6;
    msg[msg_len] = ipmb_checksum(msg + 3, msg_len - 3);//验证码2
    msg_len++;

    return msg_len;
}

/**********************************************************************
 *1-函数名称: format_ipmb_rsp
 *2-函数功能: IPMB 响应报文(解包)
 *3-输入参数:
 *           data,IPMB 命令消息
 *           size,命令消息长度
 *4-返回值:
 **********************************************************************/
static int unformat_ipmb_msg(const uint8_t *msg, uint32_t *pos,uint32_t *len,ipmb_rs_msghd *mi)
{
    msg += *pos;

    if (*len < 7)
    {
        fprintf(stderr, "Message too short\n");
        return -1;
    }

    if(ipmb_checksum(msg, *len) != 0)
    {
        fprintf(stderr, "Message checksum failure\n");
        return -1;
    }
    
    (*len)--;

//    mi->rqSA = msg[0];
//    mi->netFn = msg[1] >> 2;
//    mi->rqLUN = msg[1] & 3;
//    msg[3];//校验码1
//    mi->seq = msg[4] >> 2;
//    mi->rqLUN = msg[4] & 3;
//    mi->cmd = msg[5];
//    *pos += 6;
//    *len -= 6;

    return 0;
}
/**********************************************************************
 *1-函数名称: getDeviceId
 *2-函数功能: IPMB 响应报文(解包)
 *3-输入参数:
 *           data,IPMB 命令消息
 *           size,命令消息长度
 *4-返回值:
 **********************************************************************/
uint8_t getDeviceId(uint32_t i2c_periph, uint8_t vDestAdd, uint8_t vOwnAdd)
{
    uint8_t msg[50];
//	uint8_t i = 0;
	uint32_t dataLen = 0;
    uint8_t omsg[1] = {0};
    uint32_t omsg_len = 0;
    ipmb_rq_msghd mi={
        .rsSA  = vDestAdd,
        .netFn = DEF_APP_NETFN,
        .rsLUN = 0,
        .rqSA  = vOwnAdd,
        .rqSeq = 0,
        .rqLUN = 0,
        .cmd   = 0x01
    };

	memset(msg,0,50);//清空缓冲区
    dataLen = format_ipmb_msg(msg,omsg,omsg_len,&mi);

#define PRINT_DATA 1
#if (PRINT_DATA == 1)
	bmc_debug(PRINT_DATA, " \n %s ipmb send data: \n",__func__);
    for(int i=0;i<dataLen;i++)
    {
        bmc_debug(PRINT_DATA,"%#4.2x ",msg[i]);
    }
    bmc_debug(PRINT_DATA," \n\n");
#endif

#if (IIC_TEST_MODE == 1)
	iic_moni_master_config();
	iic_moni_start();
	iic_moni_sendbyte(vDestAdd);
	if(iic_moni_wack()) return IIC_FLAG_TIMEOUT_ERR;
	for(;i<dataLen;i++)
	{
		iic_moni_sendbyte(msg[i]);
	}
	iic_moni_nack();
	iic_moni_stop();
	bmc_debug(1, " IPMB send over\n");
	iic_moni_slave_config();
	vTaskDelay(1);
	return iic_moni_slave_read();
#else 
	return ipmb_send(i2c_periph, mi.rsSA, dataLen, msg);
#endif
}
/**********************************************************************
 *1-函数名称: IPMBTask
 *2-函数功能: freertos IPMB 任务函数
 *3-输入参数:
 *           
 *4-返回值:
 **********************************************************************/
extern uint8_t gvUsartRxBuff[8];
extern uint8_t gvUsartRxFlag;
extern uint8_t gvUsartRxLen;
uint8_t gIpmbAddrTable[12] = {0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38};
#define IPMB_TRANS_PORT 0
#define ADDR_TABLE_EN   0

extern uart_data_t data_uart[5];
void IPMBTask(void* param)
{
#if 0
	uint8_t dest_addr = 0x2a;
	uint8_t owe_addr = IIC_DEFAULT_ADDR;
	//uint8_t i = 0;
	uint8_t recv = 0;
	FlagStatus iic_data = RESET;
	FlagStatus iic_clk = RESET;

#if (IPMB_TRANS_PORT == 0) 
    uint32_t iic_periph = I2C0;
	uint32_t gpio_port = GPIOB;
	uint32_t gpio_pin  = GPIO_PIN_8;
#else 
	uint32_t iic_periph = I2C1;
	uint32_t gpio_port = GPIOF;
	uint32_t gpio_pin  = GPIO_PIN_0;
#endif
	
	vTaskDelay(5000);

#if (IIC_TEST_MODE == 0)
    iic_mode_switch(iic_periph, iic_SendMode);
#elif (IIC_TEST_MODE == 1)
    iic_moni_master_config();
#elif (IIC_TEST_MODE == 2)
    iic_mode_switch(iic_periph, iic_RecvMode);
#endif
    vTaskDelay(100);
    bmc_debug(1, "IPMBTask init OK!\r\n");

#if (IIC_TEST_MODE == 3)
    recv = getDeviceId(iic_periph, dest_addr, owe_addr);

    if(recv) {
        bmc_debug(1,"ipmb recv data = %#x \n",recv);
    } else { 
#if (IIC_TEST_MODE == 0)
		iic_mode_switch(iic_periph, iic_RecvMode);
        bmc_debug(1,"ipmb Recv Mode \n");
#endif
    }
#endif
    
    while(1)
    {
        vTaskDelay(10);
        if(gvUsartRxFlag)
        {
            gvUsartRxFlag = 0;
            dest_addr = gvUsartRxBuff[0];
            gvUsartRxLen = 0;
            
			bmc_debug(1, " %s usart0 recv dest_addr = %#x \n",__func__, dest_addr);
#if ADDR_TABLE_EN
            for(i=0;i<12;i++) 
            {
                if(dest_addr == gIpmbAddrTable[i])
                {
#endif

#if (IIC_TEST_MODE == 0)
                    iic_mode_switch(iic_periph, iic_SendMode);
#endif
					recv = getDeviceId(iic_periph, dest_addr,owe_addr);
					if(recv) {
						bmc_debug(1, "ipmb recv data = %0#x \n",recv);
					} else {
#if (IIC_TEST_MODE == 0)
						iic_mode_switch(iic_periph, iic_RecvMode);
						bmc_debug(1,"ipmb Recv Mode \n");
#endif
					}
#if ADDR_TABLE_EN
                    break;
                }
            }
#endif
        }
		if(dest_addr == 0x44) {
			iic_moni_master_config();
			// gpio_bit_set(gpio_port,gpio_pin);
			// gpio_bit_set(gpio_port,gpio_pin+1);
			gpio_bit_reset(gpio_port,gpio_pin);
			gpio_bit_reset(gpio_port,gpio_pin+1);
			iic_clk  = gpio_output_bit_get(gpio_port,gpio_pin);
			iic_data = gpio_output_bit_get(gpio_port,gpio_pin+1);
			//iic_data^iic_clk
			bmc_debug(1, "ipmb iic_data = %d, iic_clk = %d \n",
											iic_data,iic_clk);
		}
    }
#endif
	uart_init(USART1,115200);
	uart_init(UART3,115200);
	uart_init(UART6,115200);
	uart_init(UART7,115200);
	// bl24c_init();
	uint8_t i,j=0;

	while(1)
	{
		// send_string("abcdefg",strlen("abcdefg"),USART1);
		// send_string("abcdefg",strlen("abcdefg"),UART3);
		// send_string("abcdefg",strlen("abcdefg"),UART6);
		// send_string("abcdefg",strlen("abcdefg"),UART7);
		//printf("ID:%X(6 bit; high bit is high pin;)\r\n",RACK_num_get());
		vTaskDelay(500);

		// if (bl24c_Check())
		// {
		// 	printf("eeprom check erro\r\n");
		// }
		// else
		// {
		// 	printf("eeprom check success\r\n");
		// }

		data_uart[0].gvUsartRxFlag=0;
		data_uart[1].gvUsartRxFlag=0;
		data_uart[2].gvUsartRxFlag=0;
		data_uart[3].gvUsartRxFlag=0;
		data_uart[4].gvUsartRxFlag=0;

		for (i=0;i<5;i++)
		{
			if (data_uart[i].gvUsartRxLen)
			{
				printf("\r\n");
				printf("channel 0---uart0(TO DEBUG)\r\n");
				printf("channel 1---uart1(TO CPLD)\r\n");
				printf("channel 2---uart3(TO RS485)\r\n");
				printf("channel 3---uart6(TO FT1)\r\n");
				printf("channel 4---uart7(TO FT2)\r\n");
				printf("recive form channel %d:",i);
				for (j=0;j<data_uart[i].gvUsartRxLen;j++)
					printf("%c",data_uart[i].gvUsartRxBuff[j]);
				printf("\r\n");

				if (i==1)
				{
					send_string(data_uart[i].gvUsartRxBuff,data_uart[i].gvUsartRxLen,UART3);
				}

				if (i==2)
				{
					send_string(data_uart[i].gvUsartRxBuff,data_uart[i].gvUsartRxLen,USART1);
				}

				if (strcmp(data_uart[i].gvUsartRxBuff,"FT1D")==0)
				{
					gpio_bit_reset(GPIOE, GPIO_PIN_2);
				}
				if (strcmp(data_uart[i].gvUsartRxBuff,"FT2D")==0)
				{
					gpio_bit_reset(GPIOD, GPIO_PIN_12);
				}
				if (strcmp(data_uart[i].gvUsartRxBuff,"FT1U")==0)
				{
					gpio_bit_set(GPIOE, GPIO_PIN_2);
				}
				if (strcmp(data_uart[i].gvUsartRxBuff,"FT2U")==0)
				{
					gpio_bit_set(GPIOD, GPIO_PIN_12);
				}
			}
			data_uart[i].gvUsartRxLen=0;
			memset(data_uart[i].gvUsartRxBuff,0,sizeof(data_uart[i].gvUsartRxBuff));
			data_uart[i].gvUsartRxFlag=1;
		}
		
		vTaskDelay(2000);
	}
	
}

// uint8_t checkSum(uint8_t vParA, uint8_t *mMsg, uint8_t vMsgLen)
// {
//     uint8_t vCheckRes=0x00;
//     if(vParA==1)
//     {
//         vCheckRes=(mMsg[0]+mMsg[1])%256;
//     }
//     else if(vParA==2)
//     {
//         for(uint8_t vTmp=3; vTmp<(vMsgLen-1); vTmp++)
//         {
//             vCheckRes=(vCheckRes+mMsg[vTmp])%256;
//         }
//     }
//     else
//     {
//         vCheckRes=0xFF;
//     }
//     return (-vCheckRes);
// }

uint8_t test_ipmi(uint8_t vDestAdd, uint8_t vOwnAdd)
{
    uint8_t vMsgLen=0x08;           //消息长度(Bytes)
    uint8_t mIpmiSendMsg[vMsgLen];
    uint8_t vNetFnLun=0xB8;         //网络功能码与逻辑单元号
    uint8_t vHeadCheck=0x00;        //IPMI 命令头部校验
    uint8_t vRqSeqLun=0x00;         //calcRqSeqLun(vIpmiCmdID,vRqSeq);  //请求序列号与逻辑单元号,高6位为序列号,低2位为逻辑单元号
    uint8_t vCmdCode=0x01;          //命令码
    uint8_t vOtherCheck=0x00;       //命令内容检验

    mIpmiSendMsg[0]=vDestAdd;
    mIpmiSendMsg[1]=vNetFnLun;
    mIpmiSendMsg[2]=vHeadCheck;
    mIpmiSendMsg[3]=vOwnAdd;
    mIpmiSendMsg[4]=vRqSeqLun;
    mIpmiSendMsg[5]=vCmdCode;	

    //准备命令数据
    mIpmiSendMsg[6]=0x00;  
    mIpmiSendMsg[7]=vOtherCheck;
    
    vHeadCheck=checkSum(1,mIpmiSendMsg,vMsgLen); 
	vOtherCheck=checkSum(2,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
    
//    for(uint8_t i=0;i<8;i++)
//    {
//        printf("%0#x ",mIpmiSendMsg[i]);
//    }
//    printf("\r\n");
    
    uint8_t SendCmd = ipmb_send(I2C0,vDestAdd,vMsgLen,mIpmiSendMsg);
	
    return SendCmd;

//    return 0;
}

/* 
0x00	0x00	测试成功
0x10	0x10	总线持续忙（超时）
0x11	0x11	起始信号生成超时
0x12	0x12	地址发送后无应答
0x15	0x15	停止信号生成超时 */

uint8_t test_iic(uint8_t DestAdd)
{
    uint8_t vRes = 0;
    uint16_t vTmp = 0;
    
    /* ------------------------------------------- */
    //检查总线空闲
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY))
    {
        vTmp++;
        if(vTmp==0x86)
        {
            vRes=0x10;
            break;
        }
    }
    
    /* 启动 I2C 总线 */
    i2c_start_on_bus(I2C0);
    
    vTmp=0x00;
    //等待 SBSEND 被设置
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND))
    {
        vTmp++;
        if(vTmp==0x86)
        {
            vRes=0x11;
            break;
        }
    }
    
    //发送指定地址
    i2c_master_addressing(I2C0, DestAdd<<1, I2C_TRANSMITTER);
    
    vTmp=0x00;
    //等待 ADDSEND 被设置
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
    {
        vTmp++;
        if(vTmp==0x86)
        {
            vRes=0x12;
            break;
        }
    }
    
    //清除 ADDSEND 设置位
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    
    /* 发送 I2C 总线 停止 */
    i2c_stop_on_bus(I2C0);
    
    vTmp=0;
    //等待总线停止
    while(I2C_CTL0(I2C0) & 0x0200)
    {
        vTmp++;
        if(vTmp==0x86)
        {
            vRes=0x15;
            break;
        }
    }
    
    return vRes;
}


#define ZK_FLAG  1 //主控模块标识符
#define GN_FLAG  2 //功能模块标识符
#define DY_FLAG  3 //电源模块标识符
#define FQ_FLAG  4 //风扇驱动标识符

#define IPMI_A 1 
#define IPMI_B 2

#define IPMB_HEAD_CHECK 1  //IPMB命令头部校验
#define IPMB_OTHER_CHECK 2 //IPMB命令内容校验



/**********************************************************************
 *1-函数名称:checkSum
 *2-函数功能:IPMB 命令头部校验与内容校验
 *3-输入参数:vParA,校验类型
 *           mMsg,IPMB 命令消息
 *           vMsgLen,命令消息长度
 *4-返回值:
 **********************************************************************/
uint8_t checkSum(uint8_t vParA, uint8_t *mMsg, uint8_t vMsgLen)
{
   uint8_t vCheckRes=0x00;
   if(vParA==IPMB_HEAD_CHECK)
   {
       vCheckRes=(mMsg[0]+mMsg[1])%256;
   }
   else if(vParA==IPMB_OTHER_CHECK)
   {
       for(uint8_t vTmp=3; vTmp<(vMsgLen-1); vTmp++)
       {
           vCheckRes=(vCheckRes+mMsg[vTmp])%256;
       }
   }
   else
   {
       vCheckRes=0xFF;
   }
   return (-vCheckRes);
}



 /**********************************************************************
 *1-函数名称:ipmiSend
 *2-函数功能:IPMI 发送信息
 *3-输入参数:vPattern,板卡类型
 *            vIpmiID,接口编号,取值范围[IPMI_A,IPMI_B]
 *            vDestAdd,目的地址
 *            vNumInfByte,发送信息字节长度
 *            mIpmiSendMsg,发送缓冲区
 *4-返回值:1,成功;其他失败
 **********************************************************************/
uint8_t ipmiSend(uint8_t vPattern, uint8_t vIpmiID, uint16_t vDestAdd, uint8_t vNumInfByte, uint8_t* mIpmiSendMsg)
{
    uint8_t vSendFlag;

    if( vPattern==ZK_FLAG || vPattern==GN_FLAG || vPattern==DY_FLAG || vPattern==FQ_FLAG )
    {
        if(IPMI_A == vIpmiID)
        {
            vSendFlag=ipmb_send(I2C0, vDestAdd, vNumInfByte, mIpmiSendMsg);
        }
        
        if(IPMI_B == vIpmiID)
        {
            vSendFlag=ipmb_send(I2C1, vDestAdd, vNumInfByte, mIpmiSendMsg);
        }
    }
    
    return vSendFlag;
}

/* ---------------------------------------------------------------------------------------------------------------------------- */

/**********************************************************************
 *1-函数名称:fruSelfTest
 *2-函数功能:发送 IPMB 命令
 *3-输入参数:vDestAdd,目的地址;vOwnAdd,自身地址
 *4-返回值: 无
 **********************************************************************/
uint8_t fruSelfTest(uint8_t vDestAdd, uint8_t vOwnAdd)
{
    uint8_t vMsgLen         =   0x08;       //消息长度(Bytes)
    uint8_t mIpmiSendMsg[vMsgLen];
    uint8_t vNetFnLun       =   0xB8;   //网络功能码与逻辑单元号
    uint8_t vHeadCheck      =   0x00;   //IPMI 命令头部校验
    uint8_t vRqSeqLun       =   0x00;   //calcRqSeqLun(vIpmiCmdID,vRqSeq);  //请求序列号与逻辑单元号,高6位为序列号,低2位为逻辑单元号
    uint8_t vCmdCode        =   0x01;   //命令码
    uint8_t vOtherCheck     =   0x00;   //命令内容检验

    mIpmiSendMsg[0]         =   vDestAdd;
    mIpmiSendMsg[1]         =   vNetFnLun;
    mIpmiSendMsg[2]         =   vHeadCheck;
    mIpmiSendMsg[3]         =   vOwnAdd;
    mIpmiSendMsg[4]         =   vRqSeqLun;
    mIpmiSendMsg[5]         =   vCmdCode;
    
    //准备命令数据
    mIpmiSendMsg[6]         =   0x00;//FRU ID,暂时只支持1个
    mIpmiSendMsg[7]         =   vOtherCheck;

	vHeadCheck              =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck             =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]         =   vHeadCheck;
	mIpmiSendMsg[vMsgLen-1] =   vOtherCheck;
	
	uint8_t vIpmiSendFlag   =   ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}


/**********************************************************************
 *1-函数名称:setVid
 *2-函数功能:
 *3-输入参数:vDestAdd:目的地址
 *            vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t setVid(uint8_t vDestAdd, uint8_t vOwnAdd,uint32_t vVid)
{
	uint8_t vMsgLen     =   0x0A;       //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];
	uint8_t vNetFnLun   =   0xB8;       //网络功能码与逻辑单元号
	uint8_t vHeadCheck  =   0x00;       //IPMI命令头部校验
	uint8_t vRqSeqLun   =   0x00;       //请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode    =   0x02;       //命令码
	uint8_t vOtherCheck =   0x00;       //命令内容校验
	uint8_t vVidLow     =   (uint8_t)(vVid  & 0x000000FF);
	uint8_t vVidMid     =   (uint8_t)((vVid & 0x0000FF00)>>8);;
	uint8_t vVidHigh    =   (uint8_t)((vVid & 0x00FF0000)>>16);
	mIpmiSendMsg[0]     =   vDestAdd;
	mIpmiSendMsg[1]     =   vNetFnLun;
	mIpmiSendMsg[2]     =   vHeadCheck;
	mIpmiSendMsg[3]     =   vOwnAdd;
	mIpmiSendMsg[4]     =   vRqSeqLun;
	mIpmiSendMsg[5]     =   vCmdCode;
	mIpmiSendMsg[6]     =   vVidLow;
	mIpmiSendMsg[7]     =   vVidMid;
	mIpmiSendMsg[8]     =   vVidHigh;
	mIpmiSendMsg[9]     =   vOtherCheck;

	vHeadCheck              =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck             =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]         =   vHeadCheck;
	mIpmiSendMsg[vMsgLen-1] =   vOtherCheck;
	
//	uint8_t vIpmiSendFlag   =   ipmiSend(gvPattern, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg); 
    uint8_t vIpmiSendFlag   =   ipmiSend(ZK_FLAG, IPMI_B, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:setPid
 *2-函数功能:发送设置 VID
 *3-输入参数:vDestAdd,目的地址
 *           vOwnAdd,自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t setPid(uint8_t vDestAdd, uint8_t vOwnAdd,uint16_t vPid)
{
	uint8_t vMsgLen     =   0x09; //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];
	uint8_t vNetFnLun   =   0xB8; //网络功能码与逻辑单元号
	uint8_t vHeadCheck  =   0x00; //IPMI命令头部校验
	uint8_t vRqSeqLun   =   0x00; //请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode    =   0x03; //命令码
	uint8_t vOtherCheck =   0x00; //命令内容校验
	uint8_t vPidLow     =   (uint8_t)(vPid & 0x00FF);
	uint8_t vPidHigh    =   (uint8_t)((vPid & 0x0000FF00)>>8);

	mIpmiSendMsg[0]     =   vDestAdd;
	mIpmiSendMsg[1]     =   vNetFnLun;
	mIpmiSendMsg[2]     =   vHeadCheck;
	mIpmiSendMsg[3]     =   vOwnAdd;
	mIpmiSendMsg[4]     =   vRqSeqLun;
	mIpmiSendMsg[5]     =   vCmdCode;
	mIpmiSendMsg[6]     =   vPidLow;
	mIpmiSendMsg[7]     =   vPidHigh;
	mIpmiSendMsg[8]     =   vOtherCheck;

	vHeadCheck          =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck         =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]     =   vHeadCheck;
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
	
//	uint8_t vIpmiSendFlag=ipmiSend(gvPattern, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg); 
    uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_B, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:setGuid
 *2-函数功能:发送设置 VID
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t setGuid(uint8_t vDestAdd, uint8_t vOwnAdd,uint8_t * vGuid)
{
	uint8_t vMsgLen     =   0x0D;   //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];
	uint8_t vNetFnLun   =   0xB8;   //网络功能码与逻辑单元号
	uint8_t vHeadCheck  =   0x00;   //IPMI命令头部校验
	uint8_t vRqSeqLun   =   0x00;   //请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode    =   0x04;   //命令码
	uint8_t vOtherCheck =   0x00;   //命令内容检验
	
	mIpmiSendMsg[0]     =   vDestAdd;
	mIpmiSendMsg[1]     =   vNetFnLun;
	mIpmiSendMsg[2]     =   vHeadCheck;
	mIpmiSendMsg[3]     =   vOwnAdd;
	mIpmiSendMsg[4]     =   vRqSeqLun;
	mIpmiSendMsg[5]     =   vCmdCode;
    
	for(uint8_t vTemp=0;vTemp<6;vTemp++)
	{
		mIpmiSendMsg[6+vTemp]=vGuid[vTemp];
	}
    
	vHeadCheck              =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck             =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]         =   vHeadCheck;
	mIpmiSendMsg[vMsgLen-1] =   vOtherCheck;
	
//	uint8_t vIpmiSendFlag=ipmiSend(gvPattern, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	uint8_t vIpmiSendFlag   =   ipmiSend(ZK_FLAG, IPMI_B, vDestAdd, vMsgLen, mIpmiSendMsg); 
	
    return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:setDate
 *2-函数功能:发送设置Date
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t setDate(uint8_t vDestAdd, uint8_t vOwnAdd,uint8_t * vDate)
{
	uint8_t vMsgLen         =   0x11; //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];
	uint8_t vNetFnLun       =   0xB8; //网络功能码与逻辑单元号
	uint8_t vHeadCheck      =   0x00; //IPMI命令头部校验
	uint8_t vRqSeqLun       =   0x00; //请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode        =   0x07;  //命令码
	uint8_t vOtherCheck     =   0x00; //命令内容校验
	
	mIpmiSendMsg[0]         =   vDestAdd;
	mIpmiSendMsg[1]         =   vNetFnLun;
	mIpmiSendMsg[2]         =   vHeadCheck;
	mIpmiSendMsg[3]         =   vOwnAdd;
	mIpmiSendMsg[4]         =   vRqSeqLun;
	mIpmiSendMsg[5]         =   vCmdCode;
    
	for(uint8_t vTemp=0;vTemp<10;vTemp++)
	{
		mIpmiSendMsg[6+vTemp]=vDate[vTemp];
	}
    
	vHeadCheck              =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck             =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]         =   vHeadCheck;
	mIpmiSendMsg[vMsgLen-1] =   vOtherCheck;
	
//	uint8_t vIpmiSendFlag=ipmiSend(gvPattern, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg); 
	uint8_t vIpmiSendFlag   =   ipmiSend(ZK_FLAG, IPMI_B, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}


/**********************************************************************
 *1-函数名称:getDate
 *2-函数功能:发送IPMB命令 getDate
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
//uint8_t getDate(uint8_t vDestAdd, uint8_t vOwnAdd)
//{
//	uint8_t vMsgLen         =   0x07; //消息长度(Bytes)
//	uint8_t mIpmiSendMsg[vMsgLen];
//	uint8_t vNetFnLun       =   0xB8; //网络功能码与逻辑单元号
//	uint8_t vHeadCheck      =   0x00; //IPMI命令头部校验
//	uint8_t vRqSeqLun       =   0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq);//请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
//	uint8_t vCmdCode        =   0x06; //命令码
//	uint8_t vOtherCheck     =   0x00; //命令内容校验
//	
//	mIpmiSendMsg[0]         =   vDestAdd;
//	mIpmiSendMsg[1]         =   vNetFnLun;
//	mIpmiSendMsg[2]         =   vHeadCheck;
//	mIpmiSendMsg[3]         =   vOwnAdd;
//	mIpmiSendMsg[4]         =   vRqSeqLun;
//	mIpmiSendMsg[5]         =   vCmdCode;	
//	mIpmiSendMsg[6]         =   vOtherCheck;

//	vHeadCheck              =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
//	vOtherCheck             =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
//	mIpmiSendMsg[2]         =   vHeadCheck;
//	mIpmiSendMsg[vMsgLen-1] =   vOtherCheck;
//	
//	uint8_t vIpmiSendFlag   =   ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
//    
//	return vIpmiSendFlag;
//}


//test
uint8_t getDate(uint8_t vDestAdd, uint8_t vOwnAdd)
{
	uint8_t vMsgLen         =   0x07; //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];
	uint8_t vNetFnLun       =   0x2E; //网络功能码与逻辑单元号
	uint8_t vHeadCheck      =   0x00; //IPMI命令头部校验
	uint8_t vRqSeqLun       =   0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq);//请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode        =   0x31; //命令码
	uint8_t vOtherCheck     =   0x00; //命令内容校验
	
	mIpmiSendMsg[0]         =   vDestAdd;
	mIpmiSendMsg[1]         =   vNetFnLun;
	mIpmiSendMsg[2]         =   vHeadCheck;
	mIpmiSendMsg[3]         =   vOwnAdd;
	mIpmiSendMsg[4]         =   vRqSeqLun;
	mIpmiSendMsg[5]         =   vCmdCode;	
	mIpmiSendMsg[6]         =   vOtherCheck;

	vHeadCheck              =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck             =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]         =   vHeadCheck;
	mIpmiSendMsg[vMsgLen-1] =   vOtherCheck;
	
	uint8_t vIpmiSendFlag   =   ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:setEventRecevier
 *2-函数功能:发送IPMB命令 set Event Recevier 
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t setEventRecevier(uint8_t vDestAdd, uint8_t vOwnAdd)
{
	uint8_t vMsgLen     =   0x09; //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];
	uint8_t vNetFnLun   =   0x10; //网络功能码与逻辑单元号
	uint8_t vHeadCheck  =   0x00; //IPMI命令头部校验
	uint8_t vRqSeqLun   =   0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq);//请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode    =   0x00; //命令码
	uint8_t vOtherCheck =   0x00; //命令内容校验
	
	mIpmiSendMsg[0]     =   vDestAdd;
	mIpmiSendMsg[1]     =   vNetFnLun;
	mIpmiSendMsg[2]     =   vHeadCheck;
	mIpmiSendMsg[3]     =   vOwnAdd;
	mIpmiSendMsg[4]     =   vRqSeqLun;
	mIpmiSendMsg[5]     =   vCmdCode;	

	//准备命令数据
	mIpmiSendMsg[6]     =   0x20; //命令数据1,填写主控 IPMB 地址
	mIpmiSendMsg[7]     =   0x00; //命令数据2
	mIpmiSendMsg[8]     =   vOtherCheck;

	vHeadCheck          =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck         =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]     =   vHeadCheck;
    
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
	
	uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:setRs232Channel
 *2-函数功能:设置 RS232 通道,主控发送到风扇
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t setRs232Channel(uint8_t vDestAdd, uint8_t vOwnAdd, uint8_t vUartSet1, uint8_t vUartSet2)
{
	uint8_t vMsgLen         =   0x09; //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];
	uint8_t vNetFnLun       =   0xB9; //网络功能码与逻辑单元号
	uint8_t vHeadCheck      =   0x00; //IPMI命令头部校验
	uint8_t vRqSeqLun       =   0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq);//请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode        =   0x22; //命令码
	uint8_t vOtherCheck     =   0x00; //命令内容校验
	
	mIpmiSendMsg[0]         =   vDestAdd;
	mIpmiSendMsg[1]         =   vNetFnLun;
	mIpmiSendMsg[2]         =   vHeadCheck;
	mIpmiSendMsg[3]         =   vOwnAdd;
	mIpmiSendMsg[4]         =   vRqSeqLun;
    mIpmiSendMsg[5]         =   vCmdCode;	

	//准备命令数据
    mIpmiSendMsg[6]         =   vUartSet1; //0xE1; //串口1,高四位为插槽号 1~14,低四位为串口号 1~2,例如 0xE1 表示设置串口号 1 切换到 14 槽
	mIpmiSendMsg[7]         =   vUartSet2; //0xE2; //串口2,高四位为插槽号 1~14,低四位为串口号 1~2,例如 0xE2 表示设置串口号 2 切换到 14 槽
	vHeadCheck              =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck             =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]         =   vHeadCheck;
	mIpmiSendMsg[vMsgLen-1] =   vOtherCheck;
	
//	uint8_t vIpmiSendFlag=ipmiSend(gvPattern, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
    uint8_t vIpmiSendFlag   =   ipmiSend(ZK_FLAG, IPMI_B, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:getRs232Channel
 *2-函数功能:获取RS232通道设置情况,主控发送风扇
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t getRs232Channel(uint8_t vDestAdd, uint8_t vOwnAdd)
{
	uint8_t vMsgLen     =   0x08;       //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];

	uint8_t vNetFnLun   =   0xB9;       //网络功能码与逻辑单元号
	uint8_t vHeadCheck  =   0x00;       //IPMI命令头部校验

	uint8_t vRqSeqLun   =   0x00;       //calcRqSeqLun(vIpmiCmdID,vRqSeq);//请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode    =   0x21;       //命令码
	uint8_t vOtherCheck =   0x00;       //命令内容校验
	
	mIpmiSendMsg[0]     =   vDestAdd;
	mIpmiSendMsg[1]     =   vNetFnLun;
	mIpmiSendMsg[2]     =   vHeadCheck;
	mIpmiSendMsg[3]     =   vOwnAdd;
	mIpmiSendMsg[4]     =   vRqSeqLun;
    mIpmiSendMsg[5]     =   vCmdCode;	

	
	//准备命令数据
    uint8_t vFruId      =   0x01; //FRU ID
    
	mIpmiSendMsg[6]     =   vFruId;
    
	vHeadCheck          =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen);
	vOtherCheck         =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen);
	mIpmiSendMsg[2]     =   vHeadCheck;
    
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
	
	uint8_t vIpmiSendFlag   =   ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg); 
    
	return vIpmiSendFlag;
}


/**********************************************************************
 *1-函数名称:getFanProperties
 *2-函数功能:IPMB 命令 Get Fan Speed Properties
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t getFanProperties(uint8_t vDestAdd, uint8_t vOwnAdd)
{
	uint8_t vMsgLen         =   0x09; //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];

	uint8_t vNetFnLun       =   0xB0; //网络功能码与逻辑单元号
	uint8_t vHeadCheck      =   0x00; //IPMI命令头部校验

	uint8_t vRqSeqLun       =   0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq); //请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode        =   0x14; //命令码
	uint8_t vOtherCheck     =   0x00; //命令内容校验
	
	mIpmiSendMsg[0]         =   vDestAdd;
	mIpmiSendMsg[1]         =   vNetFnLun;
	mIpmiSendMsg[2]         =   vHeadCheck;
	mIpmiSendMsg[3]         =   vOwnAdd;
	mIpmiSendMsg[4]         =   vRqSeqLun;
  mIpmiSendMsg[5]         =   vCmdCode;	

	//准备命令数据
  uint8_t vVsoCmdNum      =   0x03; //VSO命令号
	uint8_t vFruId          =   0x00; //0x00:暂时只支持一个FRU
	mIpmiSendMsg[6]         =   vVsoCmdNum;
	mIpmiSendMsg[7]         =   vFruId;
	
	vHeadCheck              =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck             =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]         =   vHeadCheck;
	mIpmiSendMsg[vMsgLen-1] =   vOtherCheck;
	
	uint8_t vIpmiSendFlag   =   ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}


/**********************************************************************
 *1-函数名称:setFanLevelA
 *2-函数功能:IPMB 命令 Set Fan Level
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t setFanLevelA(uint8_t vDestAdd, uint8_t vOwnAdd,uint8_t vFanLev)
{
	uint8_t vMsgLen         =0x0A; //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];

	uint8_t vNetFnLun       =0xB0; //网络功能码与逻辑单元号
	uint8_t vHeadCheck      =0x00; //IPMI命令头部校验

	uint8_t vRqSeqLun       =0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq);//请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode        =0x15; //命令码
	uint8_t vOtherCheck     =0x00;//命令内容校验
    
	mIpmiSendMsg[0]         =vDestAdd;
	mIpmiSendMsg[1]         =vNetFnLun;
	mIpmiSendMsg[2]         =vHeadCheck;
	mIpmiSendMsg[3]         =vOwnAdd;
	mIpmiSendMsg[4]         =vRqSeqLun;
	mIpmiSendMsg[5]         =vCmdCode;	

	//准备命令数据
	uint8_t vVsoCmdNum  =   0x03; //VSO命令号
	uint8_t vFruId      =   0x00; //0x00:暂时支持一个FRU
	uint8_t vFanSpeed   =   0x10;
	
	vFanSpeed           =   vFanLev; //风扇转速,0x01~0xFD,0xFE代表关闭,默认为 0x10
	mIpmiSendMsg[6]     =   vVsoCmdNum;
	mIpmiSendMsg[7]     =   vFruId;
	mIpmiSendMsg[8]     =   vFanSpeed;
	
	vHeadCheck          =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck         =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]     =   vHeadCheck;
    
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
    
    uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:setFanLevelB
 *2-函数功能:IPMB 命令 Set Fan Level
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t setFanLevelB(uint8_t vDestAdd, uint8_t vOwnAdd,uint8_t vFanLev)
{
	uint8_t vMsgLen         =   0x0A;  //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];

	uint8_t vNetFnLun       =   0xB0;  //网络功能码与逻辑单元号
	uint8_t vHeadCheck      =   0x00;  //IPMI命令头部校验

	uint8_t vRqSeqLun       =   0x00;  //calcRqSeqLun(vIpmiCmdID,vRqSeq);//请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode        =   0x15;  //命令码
	uint8_t vOtherCheck     =   0x00;  //命令内容校验
    
	mIpmiSendMsg[0]         =   vDestAdd;
	mIpmiSendMsg[1]         =   vNetFnLun;
	mIpmiSendMsg[2]         =   vHeadCheck;
	mIpmiSendMsg[3]         =   vOwnAdd;
	mIpmiSendMsg[4]         =   vRqSeqLun;
	mIpmiSendMsg[5]         =   vCmdCode;	

	//准备命令数据
	uint8_t vVsoCmdNum      =   0x03; //VSO命令号
	uint8_t vFruId          =   0x00; //0x00:暂时支持一个FRU
	uint8_t vFanSpeed       =   0x10;
	
	vFanSpeed               =   vFanLev;//风扇转速,0x01~0xFD,0xFE代表关闭,默认为 0x10
	mIpmiSendMsg[6]         =   vVsoCmdNum;
	mIpmiSendMsg[7]         =   vFruId;
	mIpmiSendMsg[8]         =   vFanSpeed;
	
	vHeadCheck              =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck             =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]         =   vHeadCheck;
	mIpmiSendMsg[vMsgLen-1] =   vOtherCheck;

    uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_B, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:getFanLevel
 *2-函数功能:IPMB 命令 Get Fan Level
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t getFanLevel(uint8_t vDestAdd, uint8_t vOwnAdd)
{
	uint8_t vMsgLen     =   0x09;   //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];

	uint8_t vNetFnLun   =   0xB0; //网络功能码与逻辑单元号
	uint8_t vHeadCheck  =   0x00;//IPMI命令头部校验

	uint8_t vRqSeqLun   =   0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq);//请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode    =   0x16; //命令码
	uint8_t vOtherCheck =   0x00;//命令内容校验
	
	mIpmiSendMsg[0]     =   vDestAdd;
	mIpmiSendMsg[1]     =   vNetFnLun;
	mIpmiSendMsg[2]     =   vHeadCheck;
	mIpmiSendMsg[3]     =   vOwnAdd;
	mIpmiSendMsg[4]     =   vRqSeqLun;
    mIpmiSendMsg[5]     =   vCmdCode;	

	//准备命令数据
    uint8_t vVsoCmdNum  =0x03; //VSO命令号
	uint8_t vFruId      =0x00; //0x00:暂时只支持一个FRU
	mIpmiSendMsg[6]     =vVsoCmdNum;
	mIpmiSendMsg[7]     =vFruId;
	vHeadCheck          =checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck         =checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]     =vHeadCheck;
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
	
	uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:getDeviceId
 *2-函数功能:IPMB 命令 Get Device Id
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
//uint8_t getDeviceId(uint8_t vDestAdd, uint8_t vOwnAdd)
//{
//	uint8_t vMsgLen     =   0x07;  //消息长度(Bytes)
//	uint8_t mIpmiSendMsg[vMsgLen];

//	uint8_t vNetFnLun   =   0x18;  //网络功能码与逻辑单元号
//	uint8_t vHeadCheck  =   0x00;  //IPMI命令头部校验

//	uint8_t vRqSeqLun   =   0x00;  //calcRqSeqLun(vIpmiCmdID,vRqSeq); //请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
//	uint8_t vCmdCode    =   0x01;  //命令码
//	uint8_t vOtherCheck =   0x00;  //命令内容校验
//	
//	mIpmiSendMsg[0]     =   vDestAdd;
//	mIpmiSendMsg[1]     =   vNetFnLun;
//	mIpmiSendMsg[2]     =   vHeadCheck;
//	mIpmiSendMsg[3]     =   vOwnAdd;
//	mIpmiSendMsg[4]     =   vRqSeqLun;
//    mIpmiSendMsg[5]     =   vCmdCode;	
//	mIpmiSendMsg[6]     =   vOtherCheck;

//	
//	vHeadCheck          =   checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
//	vOtherCheck         =   checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
//	mIpmiSendMsg[2]     =   vHeadCheck;
//    
//	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
//	
//	uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
//    
//	return vIpmiSendFlag;
//}

/**********************************************************************
 *1-函数名称:getDeviceId
 *2-函数功能:IPMB 命令 Get Device Id
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t getDeviceGuId(uint8_t vDestAdd, uint8_t vOwnAdd)
{
	uint8_t vMsgLen=0x07; //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];

	uint8_t vNetFnLun=0x18; //网络功能码与逻辑单元号
	uint8_t vHeadCheck=0x00; //IPMI命令头部校验

	uint8_t vRqSeqLun=0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq);  //请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode=0x08; //命令码
	uint8_t vOtherCheck=0x00; //命令内容校验
	
	mIpmiSendMsg[0]=vDestAdd;
	mIpmiSendMsg[1]=vNetFnLun;
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[3]=vOwnAdd;
	mIpmiSendMsg[4]=vRqSeqLun;
    mIpmiSendMsg[5]=vCmdCode;	
	mIpmiSendMsg[6]=vOtherCheck;

	vHeadCheck=checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck=checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
	
	uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:getPowerOnTime
 *2-函数功能:IPMB 命令 Get Power Time
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t getPowerOnTime(uint8_t vDestAdd, uint8_t vOwnAdd)
{
	uint8_t vMsgLen=0x08;       //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];

	uint8_t vNetFnLun=0xB8; //网络功能码与逻辑单元号
	uint8_t vHeadCheck=0x00; //IPMI命令头部校验

	uint8_t vRqSeqLun=0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq);     //请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode=0x05; //命令码
	uint8_t vOtherCheck=0x00; //命令内容校验
	
	mIpmiSendMsg[0]=vDestAdd;
	mIpmiSendMsg[1]=vNetFnLun;
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[3]=vOwnAdd;
	mIpmiSendMsg[4]=vRqSeqLun;
    mIpmiSendMsg[5]=vCmdCode;	

	
	//准备命令数据
	mIpmiSendMsg[6]=0x00; //预留
	vHeadCheck=checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck=checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
	
	uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg); 
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:getVsoCap
 *2-函数功能:IPMB 命令 Get VSO Capablities 响应处理函数
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t getVsoCap(uint8_t vDestAdd, uint8_t vOwnAdd)
{
	uint8_t vMsgLen=0x08;       //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];

	uint8_t vNetFnLun=0xB0; //网络功能码与逻辑单元号
	uint8_t vHeadCheck=0x00; //IPMI命令头部校验

	uint8_t vRqSeqLun=0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq);  //请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode=0x00; //命令码
	uint8_t vOtherCheck=0x00; //命令内容校验
	
	mIpmiSendMsg[0]=vDestAdd;
	mIpmiSendMsg[1]=vNetFnLun;
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[3]=vOwnAdd;
	mIpmiSendMsg[4]=vRqSeqLun;
    mIpmiSendMsg[5]=vCmdCode;	

	//准备命令数据
    uint8_t vVsoCmdNum=0x03; //VSO命令号

	mIpmiSendMsg[6]=vVsoCmdNum;
	mIpmiSendMsg[7]=vOtherCheck;

	vHeadCheck=checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck=checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
	
	uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:setIpmbState
 *2-函数功能:IPMB 命令 Set Ipmb State
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
             ctrIpmbA:IPMA_A 使能控制参数
             ctrIpmbB:IPMB_B 使能控制参数
 *4-返回值:无
 **********************************************************************/
uint8_t setIpmbState(uint8_t vDestAdd, uint8_t vOwnAdd, uint8_t ctrIpmbA, uint8_t ctrIpmbB)
{
	uint8_t vMsgLen=0x0A; //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];
	
	uint8_t vNetFnLun=0xB0; //网络功能码与逻辑单元号
	uint8_t vHeadCheck=0x00; //IPMI命令头部校验
	//uint8_t vOwnAdd=sIpmiInitSet.svOwnAddress;
	uint8_t vRqSeqLun=0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq); //请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode=0x09; //命令码
	uint8_t vOtherCheck=0x00; //命令内容校验
	
	mIpmiSendMsg[0]=vDestAdd;
	mIpmiSendMsg[1]=vNetFnLun;
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[3]=vOwnAdd;
	mIpmiSendMsg[4]=vRqSeqLun;
    mIpmiSendMsg[5]=vCmdCode;	
	mIpmiSendMsg[6]=vOtherCheck;

	//准备命令数据
    uint8_t vVsoCmdNum=0x03; //VSO命令号
	mIpmiSendMsg[6]=vVsoCmdNum;
	mIpmiSendMsg[7]=ctrIpmbA; //IPMB_A控制参数 0x01 使能,0x00 失能
	mIpmiSendMsg[8]=ctrIpmbB; //IPMB_B控制参数 0x01 使能,0x00 失能
	
	vHeadCheck=checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck=checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
	
	uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);


	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:setFruActivation
 *2-函数功能:IPMB 命令 Set Ipmb State
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
             vCtrPayload:控制 Payload 上下电参数,0x01 上电
 *4-返回值:无
 **********************************************************************/
uint8_t setFruActivation(uint8_t vDestAdd, uint8_t vOwnAdd, uint8_t vCtrPayload)
{
	uint8_t vMsgLen=0x0A;           //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];

	uint8_t vNetFnLun=0xB0; //网络功能码与逻辑单元号
	uint8_t vHeadCheck=0x00; //IPMI命令头部校验

	uint8_t vRqSeqLun=0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq); //请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode=0x0C; //命令码
	uint8_t vOtherCheck=0x00; //命令内容校验
	
	mIpmiSendMsg[0]=vDestAdd;
	mIpmiSendMsg[1]=vNetFnLun;
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[3]=vOwnAdd;
	mIpmiSendMsg[4]=vRqSeqLun;
	mIpmiSendMsg[5]=vCmdCode;	
	mIpmiSendMsg[6]=vOtherCheck;

	//准备命令数据
	uint8_t vVsoCmdNum=0x03; //VSO命令号
	mIpmiSendMsg[6]=vVsoCmdNum;
	mIpmiSendMsg[7]=0x00; //FRU ID,暂时只支持1个FRU
	mIpmiSendMsg[8]=vCtrPayload; //Payload上电控制参数
	
	vHeadCheck=checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck=checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
	
//	uint8_t vIpmiSendFlag=ipmiSend(gvPattern, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);

    uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
  
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:getDeviceSdr
 *2-函数功能:IPMB 命令 Get Device SDR
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
             vRecordId:16位记录 ID
 *4-返回值:无
 **********************************************************************/
uint8_t getDeviceSdr(uint8_t vDestAdd, uint8_t vOwnAdd, uint16_t vRecordId)
{
	uint8_t vMsgLen=0x0D;           //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];
	
	uint8_t vNetFnLun=0x10; //网络功能码与逻辑单元号
	uint8_t vHeadCheck=0x00; //IPMI命令头部校验

	uint8_t vRqSeqLun=0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq); //请求序列号与逻辑单元号，高6位为序列号，低2位为逻辑单元号
	uint8_t vCmdCode=0x21; //命令码
	uint8_t vOtherCheck=0x00; //命令内容校验
	
	mIpmiSendMsg[0]=vDestAdd;
	mIpmiSendMsg[1]=vNetFnLun;
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[3]=vOwnAdd;
	mIpmiSendMsg[4]=vRqSeqLun;
    mIpmiSendMsg[5]=vCmdCode;	

	//准备命令数据
    uint8_t vRecordIdLow=((uint8_t) (vRecordId & 0x00FF)); //记录 ID 低字节
	uint8_t vRecordIdHigh=((uint8_t) ((vRecordId & 0xFF00)>>8)); //记录 ID 高字节

	mIpmiSendMsg[6]=0x00; //预留记录 ID 低 8 位
	mIpmiSendMsg[7]=0x00; //预留记录 ID 高 8 位
	mIpmiSendMsg[8]=vRecordIdLow; //记录 ID 低 8 位
	mIpmiSendMsg[9]=vRecordIdHigh; //记录 ID 高 8 位
	mIpmiSendMsg[10]=0x00; //记录内偏移量
	mIpmiSendMsg[11]=0xFF; //读取字节数,0xFF 表示读取整条数据

	vHeadCheck=checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck=checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
	
	uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg); 
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:getSensorReading
 *2-函数功能:IPMB 命令 Get Sensor Reading
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
             vSensorNum:编号
 *4-返回值:无
 **********************************************************************/
uint8_t getSensorReading(uint8_t vDestAdd, uint8_t vOwnAdd, uint8_t vSensorNum)
{
	uint8_t vMsgLen=0x08; //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];

	uint8_t vNetFnLun=0x10; //网络功能码与逻辑单元号
	uint8_t vHeadCheck=0x00; //IPMI命令头部校验

	uint8_t vRqSeqLun=0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq);  //请求序列号与逻辑单元号,高6位序列号,低2位逻辑单元号
	uint8_t vCmdCode=0x2D; //命令码
	uint8_t vOtherCheck=0x00; //命令内容检验
	
	mIpmiSendMsg[0]=vDestAdd;
	mIpmiSendMsg[1]=vNetFnLun;
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[3]=vOwnAdd;
	mIpmiSendMsg[4]=vRqSeqLun;
    mIpmiSendMsg[5]=vCmdCode;	

	
	//准备命令数据
	mIpmiSendMsg[6]=vSensorNum; //命令数据，传感器编号
	vHeadCheck=checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck=checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
	
	uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg); 
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:getManSensorNum
 *2-函数功能:IPMB 命令获得强制传感器编号
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t getManSensorNum(uint8_t vDestAdd, uint8_t vOwnAdd)
{
	uint8_t vMsgLen=0x09; //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];
	uint8_t vNetFnLun=0xB0; //网络功能码与逻辑单元号
	uint8_t vHeadCheck=0x00; //IPMI命令头部校验
	uint8_t vRqSeqLun=0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq); //请求序列号与逻辑单元号,高6位序列号,低2位逻辑单元号
	uint8_t vCmdCode=0x44; //命令码
	uint8_t vOtherCheck=0x00; //命令内容检验
	
	mIpmiSendMsg[0]=vDestAdd;
	mIpmiSendMsg[1]=vNetFnLun;
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[3]=vOwnAdd;
	mIpmiSendMsg[4]=vRqSeqLun;
    mIpmiSendMsg[5]=vCmdCode;	

	//准备命令数据
	mIpmiSendMsg[6]=0x03; //VSO命令号
	mIpmiSendMsg[7]=0x00; //FRU ID,0x00表示只支持1个FRU
	vHeadCheck=checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck=checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
	
	uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg);
    
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:getIp
 *2-函数功能:IPMB get IP
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
uint8_t getIp(uint8_t vDestAdd, uint8_t vOwnAdd)
{
	uint8_t vMsgLen=0x08; //消息长度(Bytes)
	uint8_t mIpmiSendMsg[vMsgLen];

	uint8_t vNetFnLun=0xB8; //网络功能码与逻辑单元号
	uint8_t vHeadCheck=0x00; //IPMI命令头部校验

	uint8_t vRqSeqLun=0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq);  //请求序列号与逻辑单元号,高6位序列号,低2位逻辑单元号
	uint8_t vCmdCode=0x10; //命令码
	uint8_t vOtherCheck=0x00; //命令内容检验
	
	mIpmiSendMsg[0]=vDestAdd;
	mIpmiSendMsg[1]=vNetFnLun;
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[3]=vOwnAdd;
	mIpmiSendMsg[4]=vRqSeqLun;
	mIpmiSendMsg[5]=vCmdCode;	

	//准备命令数据
	mIpmiSendMsg[6]=0x00; //预留
	vHeadCheck=checkSum(IPMB_HEAD_CHECK,mIpmiSendMsg,vMsgLen); 
	vOtherCheck=checkSum(IPMB_OTHER_CHECK,mIpmiSendMsg,vMsgLen); 
	mIpmiSendMsg[2]=vHeadCheck;
	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
	
	uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg); 
	return vIpmiSendFlag;
}

/**********************************************************************
 *1-函数名称:setIp
 *2-函数功能:IPMB Set IP
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
//uint8_t setIp(uint8_t vDestAdd, uint8_t vOwnAdd)
//{
//	uint8_t vMsgLen=0x0C; //消息长度(Bytes)
//	uint8_t mIpmiSendMsg[vMsgLen];

//	uint8_t vNetFnLun=0xBB; //网络功能码与逻辑单元号
//	uint8_t vHeadCheck=0x00; //IPMI命令头部校验

//	uint8_t vRqSeqLun=0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq);   //请求序列号与逻辑单元号,高6位序列号,低2位逻辑单元号
//	uint8_t vCmdCode=0x11; //命令码
//	uint8_t vOtherCheck=0x00; //命令内容检验
//	
//	mIpmiSendMsg[0]=vDestAdd;
//	mIpmiSendMsg[1]=vNetFnLun;
//	mIpmiSendMsg[2]=vHeadCheck;
//	mIpmiSendMsg[3]=vOwnAdd;
//	mIpmiSendMsg[4]=vRqSeqLun;
//    mIpmiSendMsg[5]=vCmdCode;
//	
//	//准备命令数据
//	uint8_t vSlotNum=ipmiAddToSlotNum(vDestAdd);
//	mIpmiSendMsg[6]=((uint8_t)(vIpTable[vSlotNum-1][0] & 0x000000FF)); //IP 字节 1 (低段)
//	mIpmiSendMsg[7]=((uint8_t)((vIpTable[vSlotNum-1][0] & 0x0000FF00)>>8)); //IP字节2
//	mIpmiSendMsg[8]=((uint8_t)((vIpTable[vSlotNum-1][0] & 0x00FF0000)>>16)); //IP字节3
//	mIpmiSendMsg[9]=((uint8_t)((vIpTable[vSlotNum-1][0] & 0xFF000000)>>24)); //IP字节4
//	mIpmiSendMsg[10]=vIpMaskTable[vSlotNum-1][0];
//	mIpmiSendMsg[2]=vHeadCheck;
//	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
//	
//  uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_B, vDestAdd, vMsgLen, mIpmiSendMsg); 
//	return vIpmiSendFlag;
//}

/**********************************************************************
 *1-函数名称:setTrapIp
 *2-函数功能:IPMB Set Trap IP
 *3-输入参数:vDestAdd:目的地址
             vOwnAdd:自身地址
 *4-返回值:无
 **********************************************************************/
//uint8_t setTrapIp(uint8_t vDestAdd, uint8_t vOwnAdd)
//{
//	uint8_t vMsgLen=0x0C; //消息长度(Bytes)
//	uint8_t mIpmiSendMsg[vMsgLen];

//	uint8_t vNetFnLun=0xB9; //网络功能码与逻辑单元号
//	uint8_t vHeadCheck=0x00; //IPMI命令头部校验

//	uint8_t vRqSeqLun=0x00; //calcRqSeqLun(vIpmiCmdID,vRqSeq);   //请求序列号与逻辑单元号,高6位序列号,低2位逻辑单元号
//	uint8_t vCmdCode=0x12; //命令码
//	uint8_t vOtherCheck=0x00; //命令内容检验
//	
//	mIpmiSendMsg[0]=vDestAdd;
//	mIpmiSendMsg[1]=vNetFnLun;
//	mIpmiSendMsg[2]=vHeadCheck;
//	mIpmiSendMsg[3]=vOwnAdd;
//	mIpmiSendMsg[4]=vRqSeqLun;
//  mIpmiSendMsg[5]=vCmdCode;	

//	
//	//准备命令数据
//	uint8_t vSlotNum=ipmiAddToSlotNum(vDestAdd);
//	mIpmiSendMsg[6]=((uint8_t)(vTrapIpTable[vSlotNum-1][0] & 0x000000FF)); //IP字节1(低段)
//	mIpmiSendMsg[7]=((uint8_t)((vTrapIpTable[vSlotNum-1][0] & 0x0000FF00)>>8)); //IP字节2
//	mIpmiSendMsg[8]=((uint8_t)((vTrapIpTable[vSlotNum-1][0] & 0x00FF0000)>>16)); //IP字节3
//	mIpmiSendMsg[9]=((uint8_t)((vTrapIpTable[vSlotNum-1][0] & 0xFF000000)>>24)); //IP字节4
//	mIpmiSendMsg[10]=0x00; //预留
//	
//	mIpmiSendMsg[2]=vHeadCheck;
//	mIpmiSendMsg[vMsgLen-1]=vOtherCheck;
//	
//	uint8_t vIpmiSendFlag=ipmiSend(ZK_FLAG, IPMI_A, vDestAdd, vMsgLen, mIpmiSendMsg); 
//	return vIpmiSendFlag;
//}


