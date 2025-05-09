#include "bsp_uart.h"
#include "bsp_gpio.h"
#include "shell_port.h"
#include "rs485.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "protocol.h"
static SemaphoreHandle_t BinarySemaphore_rs485;	//二值信号量句柄

static uint8_t recive_flag=0;
static uint8_t temp_buff[RS485_MAXBUFF_SIZE]={0};
static uint16_t temp_index=0;

void RS485_IrqHandler(void)
{
    uint8_t Rxc = 0;
    //printf("485\r\n");
    BaseType_t xHigherPriorityTaskWoken;
    if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE))
    {		
				//printf("1\r\n");
        usart_interrupt_flag_clear(UART3,USART_INT_FLAG_RBNE);
        Rxc = usart_data_receive(UART3);
			
			  // temp_buff[temp_index%RS485_MAXBUFF_SIZE]=Rxc;
        //  temp_index=(temp_index+1)%RS485_MAXBUFF_SIZE;
				//xSemaphoreGiveFromISR(BinarySemaphore_rs485,&xHigherPriorityTaskWoken);
			
        if ((Rxc==(RS485_PRO_HEAD&0xff))&&(recive_flag==0))
        {
           // printf("1\r\n");
            recive_flag=1;
        }
        if ((Rxc==(RS485_PRO_HEAD&0xff))&&(recive_flag==1))
        {
            //printf("2\r\n");
            recive_flag=2;
        }
        if (recive_flag==2)
        {
            temp_buff[temp_index%RS485_MAXBUFF_SIZE]=Rxc;
            temp_index=(temp_index+1)%RS485_MAXBUFF_SIZE;

            if (temp_buff[temp_index-1]==(0xff&RS485_PRO_TAIL))
		    {
                //printf("3\r\n");
                if (temp_buff[temp_index-RS485_PRO_TAIL_LEN]==(0xff&RS485_PRO_TAIL))
                {
                    //printf("4\r\n");
                    
                    xSemaphoreGiveFromISR(BinarySemaphore_rs485,&xHigherPriorityTaskWoken);
                   
                    recive_flag=0;
                }
		    }

        }
    }
}

uint16_t send_485msg(uint8_t *buf,uint16_t mstrlen)
{
	send_string(buf,mstrlen,UART3);
	return mstrlen;
}

uint16_t recv_485msg(uint8_t *buf)
{
    uint16_t ret=0;
	memcpy(buf,temp_buff,temp_index);
#if 0
	printf("%d\r\n",temp_index);
		int i=0;
	printf("recv data:");
	for (i=0;i<temp_index;i++)
	{
		printf("%x ",temp_buff[i]);
	}
#endif
    memset(temp_buff,0,sizeof(temp_buff));
    ret=temp_index;
    temp_index=0;
	return ret;
}

void RS485Task(void* param)
{
		uart_init(UART3,115200);
    bmc_infoinit();
    BinarySemaphore_rs485=xSemaphoreCreateBinary();
    BaseType_t err=pdFALSE;

    if (BinarySemaphore_rs485==NULL)
    {
        printf("careat BinarySemaphore_rs485 failed\r\n");
        return;
    }

		while(1)
		{
				err=xSemaphoreTake(BinarySemaphore_rs485,portMAX_DELAY);
				if(err==pdTRUE)
				{
						rs485_task_func();
				}
				else if(err==pdFALSE)
				{
						vTaskDelay(1);   	
				}
		}
}

void rs485_test()
{
    info_rs485 rs485_test;
    info_bmc test;

    char testmsg[64]="hello";

    rs485_test.cmd=CMD_GET_STAT;
    rs485_test.src=MOD_SWITCH;
    rs485_test.dst=MOD_JSDP_1;

    send_485msg(testmsg,strlen(testmsg));

    //Rs485_MsgPackSend(test,rs485_test);
    

}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
rs485, rs485_test, rs485 test);


