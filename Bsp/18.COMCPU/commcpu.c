#include "commcpu.h"
// #include "bsp_uart.h"
// #include "protocol.h"
#include "semphr.h"
#include "shell_port.h"
// #include "bsp_gpio.h"
static SemaphoreHandle_t BinarySemaphore_commcpu1;	//二值信号量句柄
static SemaphoreHandle_t BinarySemaphore_commcpu2;	//二值信号量句柄

static uint8_t recive_flag=0;

static uint8_t temp1_buff[COMMCPU_MAXBUFF_SIZE]={0};
static uint16_t temp1_index=0;

static uint8_t temp2_buff[COMMCPU_MAXBUFF_SIZE]={0};
static uint16_t temp2_index=0;

// info_bmc pro_golbalvar_comcpu={0};//不同刀片
void commcpu_test();//声明

void CommCpu_IrqHandler(uint32_t usart_periph)
{
    uint8_t *temp_buff;
    uint16_t *temp_index;
    SemaphoreHandle_t *BinarySemaphore_commcpu;
    printf("cpu1 irq111111111111\r\n");
    switch (usart_periph)
    {
        case COMCPU_CPU1:
        {
            temp_buff=temp1_buff;
            temp_index=&temp1_index;
            BinarySemaphore_commcpu=&BinarySemaphore_commcpu1;
            break;
        }
        case COMCPU_CPU2:
        {
            temp_buff=temp2_buff;
            temp_index=&temp2_index;
            BinarySemaphore_commcpu=&BinarySemaphore_commcpu2;
            break;
        }
        default:
        break;
    }

    uint8_t Rxc = 0;
    //printf("485\r\n");
    BaseType_t xHigherPriorityTaskWoken;
    if(RESET != usart_interrupt_flag_get(usart_periph, USART_INT_FLAG_RBNE))
    {
        usart_interrupt_flag_clear(usart_periph,USART_INT_FLAG_RBNE);
        Rxc = usart_data_receive(usart_periph);
        if ((Rxc==(COMMCPU_PRO_HEAD&0xff))&&(recive_flag==0))
        {
            //printf("1\r\n");
            recive_flag=1;
        }
        if ((Rxc==(COMMCPU_PRO_HEAD&0xff))&&(recive_flag==1))
        {
            //printf("2\r\n");
            recive_flag=2;
        }
        if (recive_flag==2)
        {
            temp_buff[(*temp_index)%COMMCPU_MAXBUFF_SIZE]=Rxc;
            (*temp_index)=((*temp_index)+1)%COMMCPU_MAXBUFF_SIZE;
            if (temp_buff[(*temp_index)-1]==(0xff&COMMCPU_PRO_TAIL))
		    {
                //printf("3\r\n");
                if (temp_buff[(*temp_index)-COMMCPU_PRO_TAIL_LEN]==(0xff&COMMCPU_PRO_TAIL))
                {
                    //printf("4\r\n");
                    xSemaphoreGiveFromISR(*BinarySemaphore_commcpu,&xHigherPriorityTaskWoken);
                    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                    recive_flag=0;
                }
		    }

        }
    }
}

uint16_t send_cpu1msg(uint8_t *buf,uint16_t mstrlen)
{
	send_string(buf,mstrlen,COMCPU_CPU1);
	return mstrlen;
}

uint16_t recv_cpu1msg(uint8_t *buf)
{
    uint16_t ret=0;
	memcpy(buf,temp1_buff,temp1_index);
	memset(temp1_buff,0,sizeof(temp1_buff));
    ret=temp1_index;
	temp1_index=0;
	return temp1_index;
}

// uint16_t send_cpu2msg(uint8_t *buf,uint16_t mstrlen)
// {
// 	send_string(buf,mstrlen,COMCPU_CPU2);
// 	return mstrlen;
// }

// uint16_t recv_cpu2msg(uint8_t *buf)
// {
//     uint16_t ret=0;
// 	memcpy(buf,temp2_buff,temp2_index);
// 	memset(temp2_buff,0,sizeof(temp2_buff));
//     ret=temp2_index;
//     printf("temp2_index:%d\r\n",temp2_index);
// 	temp2_index=0;
// 	return ret;
// }


extern uart_data_t *data_temp;
void ComCpu1Task(void *param)
{
    int error = 0;
    int leddown = 0;
    uint8_t Rxc = 0;

    BinarySemaphore_commcpu1 = xSemaphoreCreateBinary();

    BaseType_t err = pdFALSE;

    if (BinarySemaphore_commcpu1 == NULL)
    {
        printf("create BinarySemaphore_rs485 failed\r\n");
        return;
    }
    char buf[128] = "aabbccdd";
    while (1)
    {
//         CommCpu_MsgPackSend(COMM_CMD_GET_STAT, MOD_CPU1);

//         err = xSemaphoreTake(BinarySemaphore_commcpu1, 3000);
//         if (err == pdTRUE)
//         {
//             error = 0;
//             gpio_bit_set(GPIOE, GPIO_PIN_4); // 前面板CPU1亮
// #if 1
//             if (RACK_num_get() == 0x3D) // 数值根据槽位号待定
//             {
//                 CommCpu_MsgPackRecv(MOD_CPU1, MOD_JSDP_1);
//                 // gpio_bit_set(GPIOE, GPIO_PIN_4);              //前面板CPU1亮
//                 printf("over CommCpu_MsgPackRecv\r\n");
//                 pro_golbalvar_comcpu.bmc_jsdp_1.cpuinfo[0].statcpu = 1; // cpu1正常
//             }
//             else if (RACK_num_get() == 0x1C)
//             {
//                 CommCpu_MsgPackRecv(MOD_CPU1, MOD_JSDP_2);
//                 // gpio_bit_set(GPIOE, GPIO_PIN_4);              //前面板CPU1亮
//                 pro_golbalvar_comcpu.bmc_jsdp_2.cpuinfo[0].statcpu = 1; // cpu1正常
//             }
//             else if (RACK_num_get() == 0x3B)
//             {
//                 CommCpu_MsgPackRecv(MOD_CPU1, MOD_JSDP_3);
//                 // gpio_bit_set(GPIOE, GPIO_PIN_4);              //前面板CPU1亮
//                 pro_golbalvar_comcpu.bmc_jsdp_3.cpuinfo[0].statcpu = 1; // cpu1正常
//             }
// #endif
//         }
//         else if (err == pdFALSE)
//         {
//             // printf("CPU2 RUN ERRO\r\n");
//             leddown++;
//             if (leddown > 3)
//             {
//                 gpio_bit_reset(GPIOE, GPIO_PIN_4); // 前面板CPU1灭
//                 leddown = 0;
//             }

//             if (0 == CPLDSTATUS1())
//             {
//                 if (RACK_num_get() == 0x3D)
//                 {
//                     pro_golbalvar_comcpu.bmc_jsdp_1.cpuinfo[0].statcpu = 0; // 计算1cpu1不工作
//                                                                             //  gpio_bit_reset(GPIOE, GPIO_PIN_4); //前面板CPU1灭
//                 }
//                 else if (RACK_num_get() == 0x1c)
//                 {
//                     pro_golbalvar_comcpu.bmc_jsdp_2.cpuinfo[0].statcpu = 0; // 计算2cpu1不工作
//                                                                             //  gpio_bit_reset(GPIOE, GPIO_PIN_4); //前面板CPU1灭
//                 }
//                 else if (RACK_num_get() == 0x3B)
//                 {
//                     pro_golbalvar_comcpu.bmc_jsdp_3.cpuinfo[0].statcpu = 0; // 计算3cpu1不工作
//                                                                             //  gpio_bit_reset(GPIOE, GPIO_PIN_4); //前面板CPU1灭
//                 }
//             }
//             else if (1 == CPLDSTATUS1())
//             {
//                 error++;
//                 error = error % 30;
//                 if (0 == error)
//                 {
//                     if (RACK_num_get() == 0x3D)
//                         pro_golbalvar_comcpu.bmc_jsdp_1.cpuinfo[0].statcpu = 2; // 计算1cpu1故障
//                     else if (RACK_num_get() == 0x1c)
//                         pro_golbalvar_comcpu.bmc_jsdp_2.cpuinfo[0].statcpu = 2; // 计算2cpu1故障
//                     else if (RACK_num_get() == 0x3B)
//                         pro_golbalvar_comcpu.bmc_jsdp_3.cpuinfo[0].statcpu = 2; // 计算3cpu1故障
//                     error = 0;
//                 }
//             }
//         }
        send_cpu1msg(buf, 10);
    
        while (usart_flag_get(USART1, USART_FLAG_RBNE) == RESET)
            ;
    // return (int)usart_data_receive(USART1);
        Rxc =  usart_data_receive(USART1);

        printf("data: %s\r\n", data_temp->gvUsartRxBuff);
        // printf("len: %d\r\n", data_temp->gvUsartRxLen);
        vTaskDelay(300);
    }
}

// void ComCpu2Task(void* param)
// {
// 		int error = 0;
// 		int leddown = 0;
//     uart_init(UART7,115200);
 
//     BinarySemaphore_commcpu2=xSemaphoreCreateBinary();
//     BaseType_t err=pdFALSE;

//     if (BinarySemaphore_commcpu2==NULL)
//     {
//         printf("careat BinarySemaphore_rs485 failed\r\n");
//         return;
//     }

// 	while(1)
// 	{
// 		   CommCpu_MsgPackSend(COMM_CMD_GET_STAT,MOD_CPU2);

// 		   err=xSemaphoreTake(BinarySemaphore_commcpu2,3000);//等待3秒
//        if(err==pdTRUE)
//        {
// 						error = 0;
// 						gpio_bit_set(GPIOE, GPIO_PIN_15);//cpu2亮
// 						#if 1
//             if (RACK_num_get()==0x3D) //数值根据槽位号待定
// 						{
// 							CommCpu_MsgPackRecv(MOD_CPU2,MOD_JSDP_1);
// 							// gpio_bit_set(GPIOE, GPIO_PIN_15);                 //cpu2亮
// 							printf("over CommCpu_MsgPackRecv\r\n");
// 							pro_golbalvar_comcpu.bmc_jsdp_1.cpuinfo[1].statcpu=1;//计算1cpu2正常
// 						}
//             else if (RACK_num_get()==0x1C)
// 						{
// 							 CommCpu_MsgPackRecv(MOD_CPU2,MOD_JSDP_2);
// 							 // gpio_bit_set(GPIOE, GPIO_PIN_15);                 //cpu2亮
// 							 pro_golbalvar_comcpu.bmc_jsdp_2.cpuinfo[1].statcpu=1;//计算2cpu2正常
// 						}
//             else if (RACK_num_get()==0x3B)
// 						{
// 							 CommCpu_MsgPackRecv(MOD_CPU2,MOD_JSDP_3);
// 							 // gpio_bit_set(GPIOE, GPIO_PIN_15);                 //cpu2亮
// 							 pro_golbalvar_comcpu.bmc_jsdp_3.cpuinfo[1].statcpu=1;//计算3cpu2正常
// 						}
// 						#endif
// 				}
//         else if(err==pdFALSE)
// 				{
// 						leddown++;
// 						if(leddown > 3)
// 						{
// 								gpio_bit_reset(GPIOE, GPIO_PIN_15); //cpu2灭
// 								leddown = 0;
// 						}

// 						if(0==CPLDSTATUS2())
// 						{
// 								 if (RACK_num_get()==0x3D)
// 										pro_golbalvar_comcpu.bmc_jsdp_1.cpuinfo[1].statcpu=0;//计算1cpu2不工作
// 									 // gpio_bit_reset(GPIOE, GPIO_PIN_15);                 //cpu2灭
// 								 else if (RACK_num_get()==0x1c)
// 										pro_golbalvar_comcpu.bmc_jsdp_2.cpuinfo[1].statcpu=0;//计算2cpu2不工作
// 									 // gpio_bit_reset(GPIOE, GPIO_PIN_15);                 //cpu2灭
// 								 else if (RACK_num_get()==0x3B)
// 										pro_golbalvar_comcpu.bmc_jsdp_3.cpuinfo[1].statcpu=0;//计算3cpu2不工作
// 									 // gpio_bit_reset(GPIOE, GPIO_PIN_15);                 //cpu2灭
// 						 }
// 						 else if(1==CPLDSTATUS2())
// 						 {
// 							 error++;
// 							 error = error%30;
// 							 if(0 == error)
// 							 {
// 									if (RACK_num_get()==0x3D)
// 											pro_golbalvar_comcpu.bmc_jsdp_1.cpuinfo[1].statcpu=2;//计算1cpu2故障
// 									else if (RACK_num_get()==0x1c)
// 											pro_golbalvar_comcpu.bmc_jsdp_2.cpuinfo[1].statcpu=2;//计算2cpu2故障
// 									else if (RACK_num_get()==0x3B)
// 											pro_golbalvar_comcpu.bmc_jsdp_3.cpuinfo[1].statcpu=2;//计算3cpu2故障
// 									error = 0;
// 							 }
// 						 }					 
// 				}
//         vTaskDelay(3000);
// 	}
// }

// void commcpu_current(info_rs485 info)//判断计算刀片状态接口函数
// {
//     switch(info.src)
// 	{
// 		case MOD_SWITCH:
// 		{
		
// 			break;
//         }
//         case MOD_JSDP_1:
//         {
//            if(0==(CPLDSTATUS1())==(CPLDSTATUS2()))//关机
//             {
//                 pro_golbalvar_comcpu.bmc_jsdp_1.current=0;
//             }
//             if(1==CPLDSTATUS1()&CPLDSTATUS2())//正常
//             {
//                 pro_golbalvar_comcpu.bmc_jsdp_1.current=1;
//             }
//             if(1==CPLDSTATUS1()^CPLDSTATUS2())//故障
//             {
//                 pro_golbalvar_comcpu.bmc_jsdp_1.current=2;
//             }
//             break;
//         }
//         case MOD_JSDP_2:
//         {
//             if(0==(CPLDSTATUS1())==(CPLDSTATUS2()))//关机
//             {
//                 pro_golbalvar_comcpu.bmc_jsdp_2.current=0;
//             }
//             if(1==(CPLDSTATUS1())&(CPLDSTATUS2()))//正常
//             {
//                 pro_golbalvar_comcpu.bmc_jsdp_2.current=1;
//             }
//             if(1==(CPLDSTATUS1())^(CPLDSTATUS2()))//故障
//             {
//                 pro_golbalvar_comcpu.bmc_jsdp_2.current=2;
//             }
//             break;
//         }
//         case MOD_JSDP_3:
//         {
//              if(0==(CPLDSTATUS1())==(CPLDSTATUS2()))//关机
//             {
//                 pro_golbalvar_comcpu.bmc_jsdp_3.current=0;
//             }
//             if(1==(CPLDSTATUS1())&(CPLDSTATUS2()))//正常
//             {
//                 pro_golbalvar_comcpu.bmc_jsdp_3.current=1;
//             }
//             if(1==(CPLDSTATUS1())^(CPLDSTATUS2()))//故障
//             {
//                 pro_golbalvar_comcpu.bmc_jsdp_3.current=2;
//             }
//          break;
//         }
        
        
//     }
// }

void commcpu_test(int argc, char *argv[])
{
    char testmsg[64]="hellohello";
    int count=0;
    int Rxc = 0;
    //printf("argc %d\r\n",argc);
    // send_cpu1msg("aaaaaaaaa", 10);
    // send_cpu1msg("bbbbbbbbb", 10);

    //send_cpu1msg(testmsg, 10);
	while(1)
	{
    while (usart_flag_get(USART2, USART_FLAG_RBNE) == RESET)
        ;
    Rxc =  usart_data_receive(USART2);

    printf("rex: %x\r\n", Rxc);
    //printf("test data: %x\r\n", data_temp->gvUsartRxBuff);
    //printf("test len: %d\r\n", data_temp->gvUsartRxLen);
	}
    // switch (argc)
    // {
    //     case 1:
    //     break;

    //     case 2:
    //     {
            
    //         sscanf(argv[1],"%d",&count);

    //         switch (count)
    //         {
    //             case 0:
    //             {
    //             //    CommCpu_MsgPackSend(COMM_CMD_GET_STAT,MOD_CPU1);
	// 								 //CommCpu_MsgPackSend(COMM_CMD_SET_RESETCPU1,MOD_CPU1);
									
    //                 break;
    //             }
    //             case 1:
    //             {
    //                 // CommCpu_MsgPackSend(COMM_CMD_GET_STAT,MOD_CPU2);
    //                 break;
    //             }
    //             default:
    //             break;
    //         }
    //         printf("send to cpu%d\r\n",count);
    //     }
    //     break;

    //     default:
    //     {
    //         printf("parameter too much\r\n");
    //         break;
    //     }
    // }
}

SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN)|SHELL_CMD_DISABLE_RETURN,
cputest, commcpu_test, cputest);

