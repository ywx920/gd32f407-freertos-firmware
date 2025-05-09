#include "protocol.h"
#include <string.h>
#include "cJSON.h"
#include "bsp_uart.h"
#include "rs485.h"
#include "commcpu.h"
#include <stdlib.h>
#include "bsp_adc.h"
#include "semphr.h"
#include "bsp_time.h"

#define ZK_PROTO_DEBUG

#ifdef ZK_PROTO_DEBUG
    #define _ZK_PROTO_DEBUG(fmt, args...) fprintf(stderr,"  _ZK_PROTO_DEBUG(%s:%d):   " fmt, __func__, __LINE__, ## args)
#else
    #define _DEBUG(format,...)
#endif

extern info_bmc pro_golbalvar_comcpu;


protocol_t portol_source =
{
    .send_485 = send_485msg,
    .recv_485 = recv_485msg,
    .send_cpu1 = send_cpu1msg,
    .send_cpu2 = send_cpu2msg,
    .recv_cpu1 = recv_cpu1msg,
    .recv_cpu2 = recv_cpu2msg,
    .rs485_info.src = MOD_SWITCH,
};

static uint8_t comcpu_recv_temp[UART_MAX_LEN];
static uint8_t comcpu_send_temp[UART_MAX_LEN];
static uint8_t rs485_send_temp[UART_MAX_LEN];
static uint8_t rs485_recv_temp[UART_MAX_LEN];
static char comcpu_jsrting[UART_MAX_LEN];
static char rs485_jsrting[UART_MAX_LEN];

info_softver Msgsoftver;//软件版本结构体

int cputemp[4];

static SemaphoreHandle_t BinarySemaphore_protol_485;

void bmc_infoinit(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	BinarySemaphore_protol_485 = xSemaphoreCreateBinary();
	if (BinarySemaphore_protol_485 == NULL)
		printf("!!!!!!ERROR BinarySemaphore_protol_485 creat failed!!!!!!!\r\n");
	else
		xSemaphoreGiveFromISR(BinarySemaphore_protol_485, &xHigherPriorityTaskWoken);
    memset(&(portol_source.bmc_msg), 0, sizeof(info_bmc));
}

static inline void bmc_Msg_date()//赋值函数
{
    portol_source.bmc_msg.bmc_switch.vol12v = getAdc3()/100;
    portol_source.bmc_msg.bmc_switch.volswitch = getAdc2()/100;
}


void bmc_reply_date(info_bmc_new *reoly_date)//上报赋值函数
{
    int i = 0;
    int j = 0;
		int numlink=0;
//JSDP1 INFO SET
    reoly_date->bmc_jsdp_1.batleverr = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[0].batleverr;
    reoly_date->bmc_jsdp_1.current = portol_source.bmc_msg.bmc_jsdp_1.current;
    reoly_date->bmc_jsdp_1.disk1 = 1024;//还未上报，先暂时写个固定值
    reoly_date->bmc_jsdp_1.disk2 = 1024;//还未上报，先暂时写个固定值
    reoly_date->bmc_jsdp_1.diskerr1 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[0].diskerr;
    reoly_date->bmc_jsdp_1.diskerr2 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[1].diskerr;
    reoly_date->bmc_jsdp_1.memcpu1 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[0].memcpu;
    reoly_date->bmc_jsdp_1.memcpu2 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[1].memcpu;
    reoly_date->bmc_jsdp_1.modID = portol_source.bmc_msg.bmc_jsdp_1.modID;
    reoly_date->bmc_jsdp_1.stasta1 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[0].stasta;
    reoly_date->bmc_jsdp_1.stasta2 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[1].stasta;
    reoly_date->bmc_jsdp_1.statcpu1 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[0].statcpu;
    reoly_date->bmc_jsdp_1.statcpu2 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[1].statcpu;
    reoly_date->bmc_jsdp_1.tempcpu1 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[0].tempcpu;
    reoly_date->bmc_jsdp_1.tempcpu2 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[1].tempcpu;
    reoly_date->bmc_jsdp_1.tempcpuerr = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[0].tempcpuerr;
    reoly_date->bmc_jsdp_1.tempmod = portol_source.bmc_msg.bmc_jsdp_1.tempmod;
    cputemp[1] = portol_source.bmc_msg.bmc_jsdp_1.tempmod;
		reoly_date->bmc_jsdp_1.timsynerr = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[0].timsynerr;
		if(reoly_date->bmc_jsdp_1.timsynerr > 49)
			reoly_date->bmc_jsdp_1.timsynerr = 41;
    reoly_date->bmc_jsdp_1.uilcpu1 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[0].uilcpu;
    reoly_date->bmc_jsdp_1.uilcpu2 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[1].uilcpu;
    reoly_date->bmc_jsdp_1.uildisk1 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[0].uildisk;
    reoly_date->bmc_jsdp_1.uildisk2 = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[1].uildisk;
    reoly_date->bmc_jsdp_1.Vbatlev = portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[0].Vbatlev;
    reoly_date->bmc_jsdp_1.vol12v = portol_source.bmc_msg.bmc_jsdp_1.vol12v;
    reoly_date->bmc_jsdp_1.volbrg1 = portol_source.bmc_msg.bmc_jsdp_1.volbrg1;
    reoly_date->bmc_jsdp_1.volbrg2 = portol_source.bmc_msg.bmc_jsdp_1.volbrg2;
    reoly_date->bmc_jsdp_1.volcpu1 = portol_source.bmc_msg.bmc_jsdp_1.volcpu1;
    reoly_date->bmc_jsdp_1.volcpu2 = portol_source.bmc_msg.bmc_jsdp_1.volcpu2;

//JSDP2 INFO SET
    reoly_date->bmc_jsdp_2.batleverr = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[0].batleverr;
    reoly_date->bmc_jsdp_2.current = portol_source.bmc_msg.bmc_jsdp_2.current;
    reoly_date->bmc_jsdp_2.disk1 = 1024;//还未上报，先暂时写个固定值
    reoly_date->bmc_jsdp_2.disk2 = 1024;//还未上报，先暂时写个固定值
    reoly_date->bmc_jsdp_2.diskerr1 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[0].diskerr;
    reoly_date->bmc_jsdp_2.diskerr2 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[1].diskerr;
    reoly_date->bmc_jsdp_2.memcpu1 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[0].memcpu;
    reoly_date->bmc_jsdp_2.memcpu2 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[1].memcpu;
    reoly_date->bmc_jsdp_2.modID = portol_source.bmc_msg.bmc_jsdp_2.modID;
    reoly_date->bmc_jsdp_2.stasta1 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[0].stasta;
    reoly_date->bmc_jsdp_2.stasta2 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[1].stasta;
    reoly_date->bmc_jsdp_2.statcpu1 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[0].statcpu;
    reoly_date->bmc_jsdp_2.statcpu2 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[1].statcpu;
    reoly_date->bmc_jsdp_2.tempcpu1 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[0].tempcpu;
    reoly_date->bmc_jsdp_2.tempcpu2 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[1].tempcpu;
    reoly_date->bmc_jsdp_2.tempcpuerr = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[0].tempcpuerr;
    reoly_date->bmc_jsdp_2.tempmod = portol_source.bmc_msg.bmc_jsdp_2.tempmod;
    cputemp[2] = portol_source.bmc_msg.bmc_jsdp_2.tempmod;
		reoly_date->bmc_jsdp_2.timsynerr = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[0].timsynerr;
		if(reoly_date->bmc_jsdp_2.timsynerr > 49)
			reoly_date->bmc_jsdp_2.timsynerr = 41;
    reoly_date->bmc_jsdp_2.uilcpu1 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[0].uilcpu;
    reoly_date->bmc_jsdp_2.uilcpu2 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[1].uilcpu;
    reoly_date->bmc_jsdp_2.uildisk1 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[0].uildisk;
    reoly_date->bmc_jsdp_2.uildisk2 = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[1].uildisk;
    reoly_date->bmc_jsdp_2.Vbatlev = portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[0].Vbatlev;
    reoly_date->bmc_jsdp_2.vol12v = portol_source.bmc_msg.bmc_jsdp_2.vol12v;
    reoly_date->bmc_jsdp_2.volbrg1 = portol_source.bmc_msg.bmc_jsdp_2.volbrg1;
    reoly_date->bmc_jsdp_2.volbrg2 = portol_source.bmc_msg.bmc_jsdp_2.volbrg2;
    reoly_date->bmc_jsdp_2.volcpu1 = portol_source.bmc_msg.bmc_jsdp_2.volcpu1;
    reoly_date->bmc_jsdp_2.volcpu2 = portol_source.bmc_msg.bmc_jsdp_2.volcpu2;

//JSDP3 INFO SET
    reoly_date->bmc_jsdp_3.batleverr = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[0].batleverr;
    reoly_date->bmc_jsdp_3.current = portol_source.bmc_msg.bmc_jsdp_3.current;
    reoly_date->bmc_jsdp_3.disk1 = 1024;//还未上报，先暂时写个固定值
    reoly_date->bmc_jsdp_3.disk2 = 1024;//还未上报，先暂时写个固定值
    reoly_date->bmc_jsdp_3.diskerr1 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[0].diskerr;
    reoly_date->bmc_jsdp_3.diskerr2 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[1].diskerr;
    reoly_date->bmc_jsdp_3.memcpu1 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[0].memcpu;
    reoly_date->bmc_jsdp_3.memcpu2 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[1].memcpu;
    reoly_date->bmc_jsdp_3.modID = portol_source.bmc_msg.bmc_jsdp_3.modID;
    reoly_date->bmc_jsdp_3.stasta1 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[0].stasta;
    reoly_date->bmc_jsdp_3.stasta2 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[1].stasta;
    reoly_date->bmc_jsdp_3.statcpu1 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[0].statcpu;
    reoly_date->bmc_jsdp_3.statcpu2 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[1].statcpu;
    reoly_date->bmc_jsdp_3.tempcpu1 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[0].tempcpu;
    reoly_date->bmc_jsdp_3.tempcpu2 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[1].tempcpu;
    reoly_date->bmc_jsdp_3.tempcpuerr = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[0].tempcpuerr;
    reoly_date->bmc_jsdp_3.tempmod = portol_source.bmc_msg.bmc_jsdp_3.tempmod;
    cputemp[3] = portol_source.bmc_msg.bmc_jsdp_3.tempmod;
		reoly_date->bmc_jsdp_3.timsynerr = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[0].timsynerr;
		if(reoly_date->bmc_jsdp_3.timsynerr > 49)
			reoly_date->bmc_jsdp_3.timsynerr = 41;
    reoly_date->bmc_jsdp_3.uilcpu1 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[0].uilcpu;
    reoly_date->bmc_jsdp_3.uilcpu2 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[1].uilcpu;
    reoly_date->bmc_jsdp_3.uildisk1 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[0].uildisk;
    reoly_date->bmc_jsdp_3.uildisk2 = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[1].uildisk;
    reoly_date->bmc_jsdp_3.Vbatlev = portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[0].Vbatlev;
    reoly_date->bmc_jsdp_3.vol12v = portol_source.bmc_msg.bmc_jsdp_3.vol12v;
    reoly_date->bmc_jsdp_3.volbrg1 = portol_source.bmc_msg.bmc_jsdp_3.volbrg1;
    reoly_date->bmc_jsdp_3.volbrg2 = portol_source.bmc_msg.bmc_jsdp_3.volbrg2;
    reoly_date->bmc_jsdp_3.volcpu1 = portol_source.bmc_msg.bmc_jsdp_3.volcpu1;
    reoly_date->bmc_jsdp_3.volcpu2 = portol_source.bmc_msg.bmc_jsdp_3.volcpu2;

//SWITCH INFO SET
    reoly_date->bmc_switch.modID = portol_source.rs485_info.src;
    reoly_date->bmc_switch.modsat = portol_source.bmc_msg.bmc_switch.modsat;

    for (i = 0; i < 30; i++)
    {
        reoly_date->bmc_switch.stalink[i] = portol_source.bmc_msg.bmc_switch.switchcpuinfo.stalink1[i];
    }
		i = 32;
    for (j = 0; j < 19; j++)
    {
        reoly_date->bmc_switch.stalink[i] = portol_source.bmc_msg.bmc_switch.switchcpuinfo.stalink2[j];
				i++;
    }

		reoly_date->bmc_switch.numlink=portol_source.bmc_msg.bmc_switch.switchcpuinfo.numlink;

		if((portol_source.bmc_msg.bmc_switch.switchcpuinfo.staopt1 | portol_source.bmc_msg.bmc_switch.switchcpuinfo.staopt2) == 0)
			reoly_date->bmc_switch.staopt = 0; //正常
		else
			reoly_date->bmc_switch.staopt = 1;

		if((int8_t)portol_source.bmc_msg.bmc_switch.switchcpuinfo.tempswitch < 90)
			reoly_date->bmc_switch.tempcpuerr = 0; //不告警
		else
			reoly_date->bmc_switch.tempcpuerr = 1;

    reoly_date->bmc_switch.tempmod = (int8_t)portol_source.bmc_msg.bmc_switch.switchcpuinfo.tempmod;
    cputemp[0] = (int8_t)portol_source.bmc_msg.bmc_switch.switchcpuinfo.tempmod;
		reoly_date->bmc_switch.tempswitch = (int8_t)portol_source.bmc_msg.bmc_switch.switchcpuinfo.tempswitch;
    reoly_date->bmc_switch.vol12v = portol_source.bmc_msg.bmc_switch.vol12v;
    reoly_date->bmc_switch.volswitch = portol_source.bmc_msg.bmc_switch.volswitch;
    bmc_Msg_date();

//CCDP INFO SET
    reoly_date->bmc_csdp.modID = 7;
    reoly_date->bmc_csdp.modsat = portol_source.bmc_msg.bmc_csdp.modstat;
    reoly_date->bmc_csdp.stacpu = portol_source.bmc_msg.bmc_csdp.stacpu;
    reoly_date->bmc_csdp.tempmod = portol_source.bmc_msg.bmc_csdp.tempmod;
		reoly_date->bmc_csdp.tempcpu = portol_source.bmc_msg.bmc_csdp.tempcpu;
    reoly_date->bmc_csdp.tempwaring = portol_source.bmc_msg.bmc_csdp.tmpwaring;
    //reoly_date->bmc_csdp.modID = portol_source.bmc_msg.bmc_csdp.modID;//存储发过来的不对，这里直接给个正确值
    reoly_date->bmc_csdp.vol12v=portol_source.bmc_msg.bmc_csdp.vol12v;
    reoly_date->bmc_csdp.volcpu = portol_source.bmc_msg.bmc_csdp.volcpu;
    reoly_date->bmc_csdp.volmem = portol_source.bmc_msg.bmc_csdp.ddrvol;
    reoly_date->bmc_csdp.uilcpu = portol_source.bmc_msg.bmc_csdp.cpuusing;
    reoly_date->bmc_csdp.memcpu = portol_source.bmc_msg.bmc_csdp.cpumemusing;
    reoly_date->bmc_csdp.stabig = portol_source.bmc_msg.bmc_csdp.largessdh;
    reoly_date->bmc_csdp.stalage = portol_source.bmc_msg.bmc_csdp.largessdc;
    reoly_date->bmc_csdp.staulage = portol_source.bmc_msg.bmc_csdp.largssdu;
    reoly_date->bmc_csdp.stawarlage = !(portol_source.bmc_msg.bmc_csdp.largessdw);//大容量存储警告不确定是否要取反
    reoly_date->bmc_csdp.stasamlstatu = portol_source.bmc_msg.bmc_csdp.samallssdh;
    reoly_date->bmc_csdp.stasamllage = portol_source.bmc_msg.bmc_csdp.samallssdc;
    reoly_date->bmc_csdp.stasamlusing = portol_source.bmc_msg.bmc_csdp.samallssdu;
    reoly_date->bmc_csdp.stasamwarlage = !(portol_source.bmc_msg.bmc_csdp.samallssdw);//不确定是否要取反
    reoly_date->bmc_csdp.raid = portol_source.bmc_msg.bmc_csdp.raidmode;
    reoly_date->bmc_csdp.batlev = portol_source.bmc_msg.bmc_csdp.rtcpower;
    reoly_date->bmc_csdp.batlevwar = portol_source.bmc_msg.bmc_csdp.rtcpowerw;
    reoly_date->bmc_csdp.statuopt = portol_source.bmc_msg.bmc_csdp.lightstatus;
    reoly_date->bmc_csdp.timsynerr = portol_source.bmc_msg.bmc_csdp.timesynerror;
    reoly_date->bmc_csdp.nas1 = portol_source.bmc_msg.bmc_csdp.ssd1status;
    reoly_date->bmc_csdp.nas2 = portol_source.bmc_msg.bmc_csdp.ssd2status;
    reoly_date->bmc_csdp.nas3 = portol_source.bmc_msg.bmc_csdp.ssd3status;
    reoly_date->bmc_csdp.nas4 = portol_source.bmc_msg.bmc_csdp.ssd4status;
    reoly_date->bmc_csdp.nas5 = portol_source.bmc_msg.bmc_csdp.ssd5status;
    reoly_date->bmc_csdp.nas6 = portol_source.bmc_msg.bmc_csdp.ssd6status;
    reoly_date->bmc_csdp.nas7 = portol_source.bmc_msg.bmc_csdp.ssd7status;
    reoly_date->bmc_csdp.nas8 = portol_source.bmc_msg.bmc_csdp.ssd8status;

//DYDP INFO SET
	reoly_date->bmc_power.current270v = portol_source.bmc_msg.bmc_power.current270v;
	reoly_date->bmc_power.current28v = portol_source.bmc_msg.bmc_power.current28v;
	reoly_date->bmc_power.dymod = !(portol_source.bmc_msg.bmc_power.dymod);
	reoly_date->bmc_power.modID = portol_source.bmc_msg.bmc_power.modID;
	reoly_date->bmc_power.sta270v1 = !(portol_source.bmc_msg.bmc_power.sta270v1);
	reoly_date->bmc_power.sta270v2 = !(portol_source.bmc_msg.bmc_power.sta270v2);
	reoly_date->bmc_power.sta28v = !(portol_source.bmc_msg.bmc_power.sta28v);
	reoly_date->bmc_power.tempmod = portol_source.bmc_msg.bmc_power.tempmod;

//other info
	if(fan_freqget(1))
		reoly_date->bmc_fengshan.stafengshan1 = 0; //正常
	else
		reoly_date->bmc_fengshan.stafengshan1 = 1;
	if(fan_freqget(2))
		reoly_date->bmc_fengshan.stafengshan2 = 0; //正常
	else
		reoly_date->bmc_fengshan.stafengshan2 = 1;
	if(fan_freqget(3))
		reoly_date->bmc_fengshan.stafengshan3 = 0; //正常
	else
		reoly_date->bmc_fengshan.stafengshan3 = 1;

	reoly_date->bmc_fengshan.fengshanspeed = ((fan_freqget(1) + fan_freqget(2) + fan_freqget(3)) / 3);
	reoly_date->bmc_fengshan.fengshanspeeddate = ((fan_freqget(1) + fan_freqget(2) + fan_freqget(3)) / 3);
}

uint8_t Rs485_MsgPackSend(info_bmc bmcinfo, info_rs485 info)
{
	BaseType_t err_pro = pdFALSE;
	BaseType_t xHigherPriorityTaskWoken;
	err_pro=xSemaphoreTake(BinarySemaphore_protol_485, portMAX_DELAY);

	if (err_pro == pdTRUE)
	{
	    memset(rs485_send_temp, 0, sizeof(rs485_send_temp)); //512
	    
	    uint16_t jstrlen = 0;
	    uint16_t sendlen = RS485_PRO_LEN; //2byte(head)+2byte(tail)+1byte(srcid)+1byte(desid)+1byte(cmd)=7byte

	    *(rs485_send_temp + PRO_OFFSET_HEADF) = ZKPRO_HEAD;
	    *(rs485_send_temp + PRO_OFFSET_HEADS) = ZKPRO_HEAD;

	    *(rs485_send_temp + PRO_OFFSET_SRCID) = info.src;
	    *(rs485_send_temp + PRO_OFFSET_DSTID) = info.dst;

	    *(rs485_send_temp + PRO_OFFSET_CMD) = (uint8_t)info.cmd;

	    *(rs485_send_temp + PRO_OFFSET_TAILF - 1) = ZKPRO_TAIL;
	    *(rs485_send_temp + PRO_OFFSET_TAILS - 1) = ZKPRO_TAIL;
	    
	    portol_source.send_485(rs485_send_temp, sendlen);
	    //printf("portol_source.send_485");
		xSemaphoreGiveFromISR(BinarySemaphore_protol_485, &xHigherPriorityTaskWoken);
#if 0
	    int i = 0;
	    printf("send data:");
	    for (i = 0; i < sendlen; i++)
	    {
	        printf("%x ", rs485_send_temp[i]);
	    }
	    printf("\r\n");
#endif
	}
	else
		return -1;
    return 0;
}
//***************cjson解析*************************************************
static inline uint8_t _jsoninfo_get(cJSON **item, cJSON *obj, char *itm, cJSON *root)
{
    //_ZK_PROTO_DEBUG("\r\n");
    *item = cJSON_GetObjectItem(obj, itm);
    if (*item == NULL)
    {
        _ZK_PROTO_DEBUG("%s pro failed\r\n",itm);
        //cJSON_Delete(root);
        return 1; //JSON解析失败
    }
    return 0;
}
//******************收到rs485数据报文命令解析**********************************
uint8_t Rs485_MsgPackRecv(info_bmc *bmcinfo,u8 *recvdata)
{

    char rootemp[JSONAME_MAX_LEN] = {0};
    info_rs485 info;

    memset(rs485_recv_temp, 0, sizeof(rs485_recv_temp));
    memset(rs485_jsrting, 0, sizeof(rs485_jsrting));
    uint16_t recvlen = portol_source.recv_485(rs485_recv_temp);
    uint16_t jstrlen = recvlen - RS485_PRO_LEN;

    info.src = *(rs485_recv_temp + PRO_OFFSET_SRCID);
    info.dst = *(rs485_recv_temp + PRO_OFFSET_DSTID);
    info.cmd = (cmd_RS485) * (rs485_recv_temp + PRO_OFFSET_CMD);

    memcpy(rs485_jsrting, rs485_recv_temp + PRO_OFFSET_MSG, jstrlen);
		
		switch ( info.cmd)
		{
			case CMD_GET_DEST:
			{
				memcpy(recvdata,rs485_jsrting,jstrlen);
				break;
			}
		
		}
		
	_ZK_PROTO_DEBUG("jstrlen:%d\r\n",jstrlen);
	_ZK_PROTO_DEBUG("%s\r\n", rs485_jsrting);
#if 0
    int i = 0;
    _ZK_PROTO_DEBUG("hex printf src:0x%x dst:0x%x:\r\n",info.src,info.dst);
    for (i = 0; i < recvlen; i++)
        printf("%x ", rs485_recv_temp[i]);
	printf("\r\n");
#endif

    cJSON *item;
    cJSON *root = cJSON_Parse(rs485_jsrting); //获取JSON端点

    if (root == NULL)
    {
        _ZK_PROTO_DEBUG("0x%x to 0x%x, root find failed\r\n",info.src,info.dst);
		//_ZK_PROTO_DEBUG("%s\r\n", rs485_jsrting);
        return 1; //JSON解析失败
    }

    switch (info.src)
    {
    case MOD_SWITCH:
    {
        memcpy(rootemp, BMC_SWITCH, strlen(BMC_SWITCH));
        break;
    }
    case MOD_JSDP_1:
    {
        memcpy(rootemp, BMC_JSDP_1, strlen(BMC_JSDP_1));
        break;
    }
    case MOD_JSDP_2:
    {
        memcpy(rootemp, BMC_JSDP_2, strlen(BMC_JSDP_2));
        break;
    }
    case MOD_JSDP_3:
    {
        memcpy(rootemp, BMC_JSDP_3, strlen(BMC_JSDP_3));
        break;
    }
    case MOD_STG:
    {
        memcpy(rootemp, BMC_CCDP, strlen(BMC_CCDP));
        break;
    }
    case MOD_POWER:
    {
        memcpy(rootemp, BMC_DYDP, strlen(BMC_DYDP));
        break;
    }
    default:
        break;
    }

    cJSON *object = cJSON_GetObjectItem(root, rootemp);

    if (object == NULL)
    {
        _ZK_PROTO_DEBUG("0x%x to 0x%x, object find failed\r\n",info.src,info.dst);
		//_ZK_PROTO_DEBUG("%s\r\n", rs485_jsrting);
        cJSON_Delete(root);
        return 2; //JSON解析失败
    }

    switch (info.src)
    {
    //其他模块收到交换刀片的命令并作出相应动作
    case MOD_SWITCH:
    {
        portol_source.rs485_info.dst = info.src; //接受方的源ID即为发送方的目的ID
        portol_source.rs485_info.cmd = info.cmd; //收到的啥命令就回啥
        switch (info.cmd)
        {
        case CMD_GET_STAT:
        {
            Rs485_MsgPackSend(portol_source.bmc_msg, portol_source.rs485_info);
            break;
        }

        case CMD_SET_RESETCPU1:
        {
            CommCpu_MsgPackSend(COMM_CMD_SET_RESETCPU, MOD_CPU1);
            break;
        }

        case CMD_SET_RESETCPU2:
        {
            CommCpu_MsgPackSend(COMM_CMD_SET_RESETCPU, MOD_CPU2);
            break;
        }
#if 0
        case CMD_SET_RESETMOD:
        {
            break;
        }

        case CMD_SET_POWDOWN:
        {
            break;
        }

        case CMD_SET_POWUP:
        {
            break;
        }
#endif
        default:
            break;
        }
        break;
    }

    //交换模块收到其他模块回复的数据并存入结构体待发送给机箱外部
    case MOD_JSDP_1:
    {
        bmcinfo->bmc_jsdp_1.modID = 1;
        _jsoninfo_get(&item, object, BMC_VOL12, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.vol12v = item->valuedouble;
        //_ZK_PROTO_DEBUG("%1.lf\r\n",bmcinfo->bmc_jsdp_1.vol12v);
        _jsoninfo_get(&item, object, BMC_VOL3V3, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.vol3v3 = item->valuedouble;
        //_ZK_PROTO_DEBUG("%f\r\n",bmcinfo->bmc_jsdp_1.vol3v3);
        _jsoninfo_get(&item, object, BMC_VOLCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.volcpu1 = item->valuedouble;
        //_ZK_PROTO_DEBUG("%f\r\n",bmcinfo->bmc_jsdp_1.volcpu1);
        _jsoninfo_get(&item, object, BMC_VOLCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.volcpu2 = item->valuedouble;
        //_ZK_PROTO_DEBUG("%f\r\n",bmcinfo->bmc_jsdp_1.volcpu2);
        _jsoninfo_get(&item, object, BMC_VOLBRG1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.volbrg1 = item->valuedouble;
        //_ZK_PROTO_DEBUG("%f\r\n",bmcinfo->bmc_jsdp_1.volbrg1);
        _jsoninfo_get(&item, object, BMC_VOLBRG2, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.volbrg2 = item->valuedouble;
        //_ZK_PROTO_DEBUG("%f\r\n",bmcinfo->bmc_jsdp_1.volbrg2);
        _jsoninfo_get(&item, object, BMC_CURRENT, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.current = item->valuedouble;
        //_ZK_PROTO_DEBUG("%f\r\n",bmcinfo->bmc_jsdp_1.current);
        _jsoninfo_get(&item, object, BMC_TEMPMOD, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.tempmod = item->valuedouble;
        //_ZK_PROTO_DEBUG("%f\r\n",bmcinfo->bmc_jsdp_1.tempmod);

        //cpu1
        _jsoninfo_get(&item, object, BMC_STATCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[0].statcpu = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].statcpu);
        _jsoninfo_get(&item, object, BMC_UILCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[0].uilcpu = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].uilcpu);
        _jsoninfo_get(&item, object, BMC_STASTAT1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[0].stasta = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].stasta);
        _jsoninfo_get(&item, object, BMC_MEMCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[0].memcpu = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].memcpu);
        _jsoninfo_get(&item, object, BMC_TEMPCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[0].tempcpu = item->valuedouble;
        //_ZK_PROTO_DEBUG("%f\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].tempcpu);
        //新增
        _jsoninfo_get(&item, object, BMC_BATLEV1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[0].Vbatlev = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].Vbatlev);
        _jsoninfo_get(&item, object, BMC_TIMSYNERR1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[0].timsynerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].timsynerr);
        _jsoninfo_get(&item, object, BMC_SOFTVER1, root);
            //return 3;
        strcpy(Msgsoftver.jsdp1_soft, item->valuestring);
        //_ZK_PROTO_DEBUG("%s\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].softver);
        //新增
        _jsoninfo_get(&item, object, BMC_UILDISK1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[0].uildisk = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].uildisk);
        _jsoninfo_get(&item, object, BMC_TEMPCPUERR1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[0].tempcpuerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].tempcpuerr);
        _jsoninfo_get(&item, object, BMC_DISKERR1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[0].diskerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].diskerr);
        _jsoninfo_get(&item, object, BMC_BATLEVERR1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[0].batleverr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].batleverr);



        //cpu2
        _jsoninfo_get(&item, object, BMC_STATCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[1].statcpu = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[1].statcpu);
        _jsoninfo_get(&item, object, BMC_UILCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[1].uilcpu = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[1].uilcpu);
        _jsoninfo_get(&item, object, BMC_STASTAT2, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[1].stasta = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[1].stasta);
        _jsoninfo_get(&item, object, BMC_MEMCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[1].memcpu = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[1].memcpu);
        _jsoninfo_get(&item, object, BMC_TEMPCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[1].tempcpu = item->valueint;
        //_ZK_PROTO_DEBUG("%f\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[1].tempcpu);
        //新增
        _jsoninfo_get(&item, object, BMC_BATLEV2, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[1].Vbatlev = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[1].Vbatlev);
        _jsoninfo_get(&item, object, BMC_TIMSYNERR2, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[1].timsynerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[1].timsynerr);
        _jsoninfo_get(&item, object, BMC_SOFTVER2, root);
            //return 3;
        strcpy(Msgsoftver.jsdp1_soft, item->valuestring);
        //_ZK_PROTO_DEBUG("%s\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[1].softver);
        //新增
        _jsoninfo_get(&item, object, BMC_UILDISK2, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[1].uildisk = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[1].uildisk);
        _jsoninfo_get(&item, object, BMC_TEMPCPUERR2, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[1].tempcpuerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[1].tempcpuerr);
        _jsoninfo_get(&item, object, BMC_DISKERR2, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[1].diskerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[1].diskerr);
        _jsoninfo_get(&item, object, BMC_BATLEVERR2, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[1].batleverr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[1].batleverr);
        break;

    }
    case MOD_JSDP_2:
    {
        bmcinfo->bmc_jsdp_2.modID = 2;
        _jsoninfo_get(&item, object, BMC_VOL12, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.vol12v = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_VOL3V3, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.vol3v3 = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_VOLCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.volcpu1 = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_VOLCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.volcpu2 = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_VOLBRG1, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.volbrg1 = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_VOLBRG2, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.volbrg2 = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_CURRENT, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.current = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_TEMPMOD, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.tempmod = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_STATCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[0].statcpu = item->valueint;

        _jsoninfo_get(&item, object, BMC_UILCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[0].uilcpu = item->valueint;

        _jsoninfo_get(&item, object, BMC_STASTAT1, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[0].stasta = item->valueint;

        _jsoninfo_get(&item, object, BMC_MEMCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[0].memcpu = item->valueint;

        _jsoninfo_get(&item, object, BMC_TEMPCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[0].tempcpu = item->valuedouble;

        //新增
        _jsoninfo_get(&item, object, BMC_BATLEV1, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[0].Vbatlev = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[0].Vbatlev);
        _jsoninfo_get(&item, object, BMC_TIMSYNERR1, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[0].timsynerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[0].timsynerr);
        _jsoninfo_get(&item, object, BMC_SOFTVER1, root);
            //return 3;
        strcpy(Msgsoftver.jsdp2_soft, item->valuestring);
        //_ZK_PROTO_DEBUG("%s\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[0].softver);
        //新增
        _jsoninfo_get(&item, object, BMC_UILDISK1, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[0].uildisk = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[0].uildisk);
        _jsoninfo_get(&item, object, BMC_TEMPCPUERR1, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[0].tempcpuerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[0].tempcpuerr);
        _jsoninfo_get(&item, object, BMC_DISKERR1, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[0].diskerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[0].diskerr);
        _jsoninfo_get(&item, object, BMC_BATLEVERR1, root);
            //return 3;
        bmcinfo->bmc_jsdp_1.cpuinfo[0].batleverr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[0].batleverr);




        _jsoninfo_get(&item, object, BMC_STATCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[1].statcpu = item->valueint;

        _jsoninfo_get(&item, object, BMC_UILCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[1].uilcpu = item->valueint;

        _jsoninfo_get(&item, object, BMC_STASTAT2, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[1].stasta = item->valueint;

        _jsoninfo_get(&item, object, BMC_MEMCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[1].memcpu = item->valueint;

        _jsoninfo_get(&item, object, BMC_TEMPCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[1].tempcpu = item->valuedouble;
        //新增
        _jsoninfo_get(&item, object, BMC_BATLEV2, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[1].Vbatlev = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[1].Vbatlev);
        _jsoninfo_get(&item, object, BMC_TIMSYNERR2, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[1].timsynerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[1].timsynerr);
        _jsoninfo_get(&item, object, BMC_SOFTVER2, root);
            //return 3;
        strcpy(Msgsoftver.jsdp2_soft, item->valuestring);
        //_ZK_PROTO_DEBUG("%s\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[1].softver);
        //新增
        _jsoninfo_get(&item, object, BMC_UILDISK2, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[1].uildisk = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[1].uildisk);
        _jsoninfo_get(&item, object, BMC_TEMPCPUERR2, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[1].tempcpuerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[1].tempcpuerr);
        _jsoninfo_get(&item, object, BMC_DISKERR2, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[1].diskerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[1].diskerr);
        _jsoninfo_get(&item, object, BMC_BATLEVERR2, root);
            //return 3;
        bmcinfo->bmc_jsdp_2.cpuinfo[1].batleverr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_2.cpuinfo[1].batleverr);

        break;
    }
    case MOD_JSDP_3:
    {
        bmcinfo->bmc_jsdp_3.modID = 3;
        _jsoninfo_get(&item, object, BMC_VOL12, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.vol12v = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_VOL3V3, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.vol3v3 = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_VOLCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.volcpu1 = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_VOLCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.volcpu2 = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_VOLBRG1, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.volbrg1 = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_VOLBRG2, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.volbrg2 = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_CURRENT, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.current = item->valuedouble;

        _jsoninfo_get(&item, object, BMC_TEMPMOD, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.tempmod = item->valuedouble;
        //cpu2

        _jsoninfo_get(&item, object, BMC_STATCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[0].statcpu = item->valueint;

        _jsoninfo_get(&item, object, BMC_UILCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[0].uilcpu = item->valueint;

        _jsoninfo_get(&item, object, BMC_STASTAT1, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[0].stasta = item->valueint;

        _jsoninfo_get(&item, object, BMC_MEMCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[0].memcpu = item->valueint;

        _jsoninfo_get(&item, object, BMC_TEMPCPU1, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[0].tempcpu = item->valuedouble;

        //新增
        _jsoninfo_get(&item, object, BMC_BATLEV1, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[0].Vbatlev = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].Vbatlev);
        _jsoninfo_get(&item, object, BMC_TIMSYNERR1, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[0].timsynerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].timsynerr);
        _jsoninfo_get(&item, object, BMC_SOFTVER1, root);
            //return 3;
        strcpy(Msgsoftver.jsdp3_soft, item->valuestring);
        //_ZK_PROTO_DEBUG("%s\r\n",bmcinfo->bmc_jsdp_1.cpuinfo[0].softver);

        //新增
        _jsoninfo_get(&item, object, BMC_UILDISK1, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[0].uildisk = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_3.cpuinfo[0].uildisk);
        _jsoninfo_get(&item, object, BMC_TEMPCPUERR1, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[0].tempcpuerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_3.cpuinfo[0].tempcpuerr);
        _jsoninfo_get(&item, object, BMC_DISKERR1, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[0].diskerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_3.cpuinfo[0].diskerr);
        _jsoninfo_get(&item, object, BMC_BATLEVERR1, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[0].batleverr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_3.cpuinfo[0].batleverr);

//cpu2
        _jsoninfo_get(&item, object, BMC_STATCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[1].statcpu = item->valueint;

        _jsoninfo_get(&item, object, BMC_UILCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[1].uilcpu = item->valueint;

        _jsoninfo_get(&item, object, BMC_STASTAT2, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[1].stasta = item->valueint;

        _jsoninfo_get(&item, object, BMC_MEMCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[1].memcpu = item->valueint;

        _jsoninfo_get(&item, object, BMC_TEMPCPU2, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[1].tempcpu = item->valuedouble;
        //新增
        _jsoninfo_get(&item, object, BMC_BATLEV2, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[1].Vbatlev = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_3.cpuinfo[1].Vbatlev);
        _jsoninfo_get(&item, object, BMC_TIMSYNERR2, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[1].timsynerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_3.cpuinfo[1].timsynerr);
        _jsoninfo_get(&item, object, BMC_SOFTVER2, root);
            //return 3;
        strcpy(Msgsoftver.jsdp3_soft, item->valuestring);
        //_ZK_PROTO_DEBUG("%s\r\n",bmcinfo->bmc_jsdp_3.cpuinfo[1].softver);
        //新增
        _jsoninfo_get(&item, object, BMC_UILDISK2, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[1].uildisk = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_3.cpuinfo[1].uildisk);
        _jsoninfo_get(&item, object, BMC_TEMPCPUERR2, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[1].tempcpuerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_3.cpuinfo[1].tempcpuerr);
        _jsoninfo_get(&item, object, BMC_DISKERR2, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[1].diskerr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_3.cpuinfo[1].diskerr);
        _jsoninfo_get(&item, object, BMC_BATLEVERR2, root);
            //return 3;
        bmcinfo->bmc_jsdp_3.cpuinfo[1].batleverr = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_jsdp_3.cpuinfo[1].batleverr);

        break;
    }
    case MOD_STG://存储
    {
        bmcinfo->bmc_csdp.modID = 7;
        _jsoninfo_get(&item, object, "TEMPCPU", root);
            //return 3;
        bmcinfo->bmc_csdp.tempcpu = item->valuedouble;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.tempcpu);

        _jsoninfo_get(&item, object, "TEMPMOD", root);
            //return 3;
        bmcinfo->bmc_csdp.tempmod = item->valuedouble;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.tempmod);

        _jsoninfo_get(&item, object, "VOL12", root);
            //return 3;
        bmcinfo->bmc_csdp.vol12v = item->valuedouble;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.vol12v);

        _jsoninfo_get(&item, object, "VOLCPU", root);
            //return 3;
        bmcinfo->bmc_csdp.volcpu = item->valuedouble;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.volcpu);

        _jsoninfo_get(&item, object, "MODSTATUS", root);
            //return 3;
        bmcinfo->bmc_csdp.modstat = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.volmem);

        _jsoninfo_get(&item, object, "CPUSTATUS", root);
            //return 3;
        bmcinfo->bmc_csdp.stacpu = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.stabig);

        _jsoninfo_get(&item, object, "TEMPWARING", root);
            //return 3;
        bmcinfo->bmc_csdp.tmpwaring = item->valuedouble;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.stasmal);

        _jsoninfo_get(&item, object, "DDRVOL", root);
            //return 3;
        bmcinfo->bmc_csdp.ddrvol = item->valuedouble;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.ulibig);
        _jsoninfo_get(&item, object, "CPUUSING", root);
            //return 3;
        bmcinfo->bmc_csdp.cpuusing = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.ulismal);
        _jsoninfo_get(&item, object, "CPUMEMUSING", root);
            //return 3;
        bmcinfo->bmc_csdp.cpumemusing = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.batlev);
        _jsoninfo_get(&item, object, "LARGESSDH", root);
            //return 3;
        bmcinfo->bmc_csdp.largessdh = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.staopt);

        _jsoninfo_get(&item, object, "LARGESSDC", root);
            //return 3;
        
        bmcinfo->bmc_csdp.largessdc = item->valueint;

        _jsoninfo_get(&item, object, "LARGESSDU", root);
            //return 3;
        bmcinfo->bmc_csdp.largssdu = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.timsynerr);
        _jsoninfo_get(&item, object, "LARGESSDW", root);
            //return 3;
        bmcinfo->bmc_csdp.largessdw = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.ccmod);
        _jsoninfo_get(&item, object, "SAMALLSSDH", root);
            //return 3;
        bmcinfo->bmc_csdp.samallssdh = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.raid);
        _jsoninfo_get(&item, object, "SAMALLSSDC", root);
            //return 3;
        bmcinfo->bmc_csdp.samallssdc = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.nas1);
        _jsoninfo_get(&item, object, "SAMALLSSDU", root);
            //return 3;
        bmcinfo->bmc_csdp.samallssdu = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.nas2);
        _jsoninfo_get(&item, object, "SAMALLSSDW", root);
            //return 3;
        bmcinfo->bmc_csdp.samallssdw = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.nas3);
        _jsoninfo_get(&item, object, "RAIDMODE", root);
            //return 3;
        bmcinfo->bmc_csdp.raidmode = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.nas4);
        _jsoninfo_get(&item, object, "RTCPOWER", root);
            //return 3;
        bmcinfo->bmc_csdp.rtcpower = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.nas5);
        _jsoninfo_get(&item, object, "RTCPOWERW", root);
            //return 3;
        bmcinfo->bmc_csdp.rtcpowerw = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.nas6);
        _jsoninfo_get(&item, object, "LIGHTSTATUS", root);
            //return 3;
        bmcinfo->bmc_csdp.lightstatus = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.nas7);
        _jsoninfo_get(&item, object, "TIMESYNERROR", root);
            //return 3;
        bmcinfo->bmc_csdp.timesynerror = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.nas8);

        _jsoninfo_get(&item, object, "SSD1STATUS", root);
            //return 3;
        bmcinfo->bmc_csdp.ssd1status = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.tempcpuerr);
        _jsoninfo_get(&item, object, "SSD2STATUS", root);
            //return 3;
        bmcinfo->bmc_csdp.ssd2status = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.stacpu);

        _jsoninfo_get(&item, object, "SSD3STATUS", root);
            //return 3;
        bmcinfo->bmc_csdp.ssd3status = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.uilcpu);
        _jsoninfo_get(&item, object, "SSD4STATUS", root);
            //return 3;
        bmcinfo->bmc_csdp.ssd4status = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.memcpu);

        _jsoninfo_get(&item, object, "SSD5STATUS", root);
            //return 3;
        bmcinfo->bmc_csdp.ssd5status = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.membig);

        _jsoninfo_get(&item, object, "SSD6STATUS", root);
            //return 3;
        bmcinfo->bmc_csdp.ssd6status = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.ulibig);
        _jsoninfo_get(&item, object, "SSD7STATUS", root);
            //return 3;
        bmcinfo->bmc_csdp.ssd7status = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.ulibigerr);
        _jsoninfo_get(&item, object, "SSD8STATUS", root);
            //return 3;
        bmcinfo->bmc_csdp.ssd8status = item->valueint;
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_csdp.memsmal);

        break;
    }
    case MOD_POWER://电源
    {
        bmcinfo->bmc_power.modID = 6;
        _jsoninfo_get(&item, object, BMC_STA270V1, root);
            //return 3;
        bmcinfo->bmc_power.sta270v1 = ((item->valueint));
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_power.sta270v1);

        _jsoninfo_get(&item, object, BMC_STA270V2, root);
            //return 3;
        bmcinfo->bmc_power.sta270v2 = ((item->valueint));
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_power.sta270v2);

        _jsoninfo_get(&item, object, BMC_STA28V, root);
            //return 3;
        bmcinfo->bmc_power.sta28v = ((item->valueint));
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_power.sta28v);

        _jsoninfo_get(&item, object, BMC_CURR270V, root);
            //return 3;
				bmcinfo->bmc_power.current270v = ((item->valuedouble) * 270) * 10; //270v功率
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_power.current270v);

        _jsoninfo_get(&item, object, BMC_CURR28V, root);
            //return 3;
        bmcinfo->bmc_power.current28v = ((item->valuedouble) * 28) * 10; //28v功率
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_power.current28v);

        _jsoninfo_get(&item, object, BMC_TEMPMOD, root);
            //return 3;
        bmcinfo->bmc_power.tempmod = ((item->valueint));
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_power.tempmod);

        _jsoninfo_get(&item, object, BMC_DYMOD, root);
            //return 3;
        bmcinfo->bmc_power.dymod = ((item->valueint));
        //_ZK_PROTO_DEBUG("%d\r\n",bmcinfo->bmc_power.dymod);

        _jsoninfo_get(&item, object, BMC_SOFTVER, root);
            //return 3;
        strcpy(Msgsoftver.power_soft, item->valuestring);
        //_ZK_PROTO_DEBUG("%s\r\n",bmcinfo->bmc_power.softver);
        break;
    }
    default:
        break;
    }

	
    cJSON_Delete(root);
	
	
	
    return 0;
}

uint8_t rs485_task_func(u8 *recvdata)
{
    uint8_t ret = 0;

    ret = Rs485_MsgPackRecv(&(portol_source.bmc_msg),recvdata);

    return ret;
}

//*************************************串口协议给cpu发送打包数据只含命令***********************************
uint8_t CommCpu_MsgPackSend(cmd_CommCpu cmd, choice_cpu cpu_choice)
{
    //printf("this is uart send cpu");
    uint16_t sendlen = COMM_PRO_LEN; //2byte(head)+2byte(tail)+1byte(srcid)=5byte
    uint8_t ret = 0;

    memset(comcpu_send_temp, 0, sizeof(comcpu_send_temp)); //512

    *(comcpu_send_temp + PRO_COMM_OFFSET_HEADF) = ZKPRO_HEAD;
    *(comcpu_send_temp + PRO_COMM_OFFSET_HEADS) = ZKPRO_HEAD;
    *(comcpu_send_temp + PRO_COMM_OFFSET_SRCID) = cmd;
    *(comcpu_send_temp + PRO_COMM_OFFSET_TAILF - 1) = ZKPRO_TAIL; //decline 1 because pack send don't hanve data
    *(comcpu_send_temp + PRO_COMM_OFFSET_TAILS - 1) = ZKPRO_TAIL; //decline 1 because pack send don't hanve data

    switch (cpu_choice)
    {
    case MOD_CPU1:
    {
        ret = portol_source.send_cpu1(comcpu_send_temp, sendlen);
        break;
    }

    case MOD_CPU2:
    {
        //printf("this is uart send cpu2");
        ret = portol_source.send_cpu2(comcpu_send_temp, sendlen);
        break;
    }

    default:
        break;
    }

    return ret;
}


//**************************串口协议收到cpu数据并解析存放到相应结构体****************************************
uint8_t CommCpu_MsgPackRecv(choice_cpu cpu_choice, mod_Type mod_choice)
{

//  uint8_t comcpu_recv_temp[UART_MAX_LEN]={0};

//  char comcpu_jsrting[UART_MAX_LEN]={0};

    memset(comcpu_recv_temp, 0, UART_MAX_LEN);
    memset(comcpu_jsrting, 0, UART_MAX_LEN);
    char rootemp[JSONAME_MAX_LEN] = {0};
//  char *rootemp=(char*)malloc(JSONAME_MAX_LEN);
    uint8_t ret = 0;
    void *cpuinfo_tmep;
    uint16_t recvlen = 0;
    uint16_t sendlen = COMM_PRO_LEN;
    switch (cpu_choice)
    {
    case MOD_CPU1:
    {
        recvlen = portol_source.recv_cpu1(comcpu_recv_temp);
        break;
    }

    case MOD_CPU2:
    {
        recvlen = portol_source.recv_cpu2(comcpu_recv_temp);
        //_ZK_PROTO_DEBUG("%d\r\n",recvlen);
        break;
    }

    default:
        break;

    }
    uint16_t jstrlen = recvlen - COMM_PRO_LEN;
    memcpy(comcpu_jsrting, comcpu_recv_temp + PRO_COMM_OFFSET_MSG, jstrlen);
    _ZK_PROTO_DEBUG("%s\r\n",comcpu_jsrting);

    cJSON *item = NULL;
    cJSON *root = cJSON_Parse(comcpu_jsrting); //获取JSON端点

    if (root == NULL)
    {
        cJSON_Delete(root);
        _ZK_PROTO_DEBUG("root find failed,is NULL\r\n");
        return 1; //JSON解析失败
    }

    switch (cpu_choice)
    {

    case MOD_CPU1:
    {
        switch (mod_choice)
        {
        case MOD_JSDP_1:
        {
            memcpy(rootemp, BMC_JSDP, strlen(BMC_JSDP));
            cpuinfo_tmep = &(portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[0]);
            break;
        }
        case MOD_JSDP_2:
        {
            memcpy(rootemp, BMC_JSDP, strlen(BMC_JSDP));
            cpuinfo_tmep = &(portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[0]);
            break;
        }
        case MOD_JSDP_3:
        {
            memcpy(rootemp, BMC_JSDP, strlen(BMC_JSDP));
            cpuinfo_tmep = &(portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[0]);
            break;
        }
        case MOD_SWITCH:
        {
            memcpy(rootemp, BMC_SWITCH, strlen(BMC_SWITCH));
            cpuinfo_tmep = &(portol_source.bmc_msg.bmc_switch.switchcpuinfo);
            break;
        }
        default:
            break;
        }
        break;
    }

    case MOD_CPU2:
    {
        switch (mod_choice)
        {
        case MOD_JSDP_1:
        {
            memcpy(rootemp, BMC_JSDP, strlen(BMC_JSDP));
            cpuinfo_tmep = &(portol_source.bmc_msg.bmc_jsdp_1.cpuinfo[1]);
            break;
        }
        case MOD_JSDP_2:
        {
            memcpy(rootemp, BMC_JSDP, strlen(BMC_JSDP));
            cpuinfo_tmep = &(portol_source.bmc_msg.bmc_jsdp_2.cpuinfo[1]);
            break;
        }
        case MOD_JSDP_3:
        {
            memcpy(rootemp, BMC_JSDP, strlen(BMC_JSDP));
            cpuinfo_tmep = &(portol_source.bmc_msg.bmc_jsdp_3.cpuinfo[1]);
            break;
        }
        case MOD_SWITCH:
        {
            memcpy(rootemp, BMC_SWITCH, strlen(BMC_SWITCH));
            cpuinfo_tmep = &(portol_source.bmc_msg.bmc_switch.switchcpuinfo);
            break;
        }
        default:
            break;
        }
    }

    default:
        break;
    }

    cJSON *object = cJSON_GetObjectItem(root, rootemp);
    if (object == NULL)
    {
        cJSON_Delete(root);
        _ZK_PROTO_DEBUG("object find failed\r\n");
        return 1;
    }

    switch (cpu_choice)
    {
    case MOD_CPU1:
    {
        if ((mod_choice == MOD_JSDP_1) || (mod_choice == MOD_JSDP_2) || (mod_choice == MOD_JSDP_3))
        {
            _jsoninfo_get(&item, object, BMC_TEMPCPU, root);
                //return 3;
            ((info_jsdpcpu *)cpuinfo_tmep)->tempcpu = item->valuedouble;

            _jsoninfo_get(&item, object, BMC_STATCPU, root);
                //return 3;
            ((info_jsdpcpu *)cpuinfo_tmep)->statcpu = item->valueint;

            _jsoninfo_get(&item, object, BMC_UILCPU, root);
                //return 3;
            ((info_jsdpcpu *)cpuinfo_tmep)->uilcpu = item->valueint;

            _jsoninfo_get(&item, object, BMC_STATSTAT, root);
                //return 3;
            ((info_jsdpcpu *)cpuinfo_tmep)->statcpu = item->valueint;

            _jsoninfo_get(&item, object, BMC_MEMCPU, root);
                //return 3;
            ((info_jsdpcpu *)cpuinfo_tmep)->memcpu = item->valueint;
        }
        else if (mod_choice == MOD_SWITCH)
        {

            _jsoninfo_get(&item, object, BMC_TEMPMOD, root);
                //return 3;
            ((info_switchcpu *)cpuinfo_tmep)->tempmod = item->valuedouble;
            _ZK_PROTO_DEBUG("%f\r\n", ((info_switchcpu *)cpuinfo_tmep)->tempmod);

            _jsoninfo_get(&item, object, BMC_TEMPSWITCH, root);
                //return 3;
            ((info_switchcpu *)cpuinfo_tmep)->tempswitch = item->valuedouble;
            _ZK_PROTO_DEBUG("%f\r\n", ((info_switchcpu *)cpuinfo_tmep)->tempswitch);

            _jsoninfo_get(&item, object, BMC_STAOPT1, root);
                //return 3;
            ((info_switchcpu *)cpuinfo_tmep)->staopt1 = item->valueint;
            _ZK_PROTO_DEBUG("%d\r\n", ((info_switchcpu *)cpuinfo_tmep)->staopt1);

            _jsoninfo_get(&item, object, BMC_STAOPT2, root);
                //return 3;
            ((info_switchcpu *)cpuinfo_tmep)->staopt2 = item->valueint;
            _ZK_PROTO_DEBUG("%d\r\n", ((info_switchcpu *)cpuinfo_tmep)->staopt2);

        }
        else if (mod_choice == MOD_POWER)
        {
            ;
        }
        else if (mod_choice == MOD_STG)
        {
            ;
        }

    }

    case MOD_CPU2:
    {
        if ((mod_choice == MOD_JSDP_1) || (mod_choice == MOD_JSDP_2) || (mod_choice == MOD_JSDP_3))
        {
            _jsoninfo_get(&item, object, BMC_TEMPCPU, root);
                //return 3;
            ((info_jsdpcpu *)cpuinfo_tmep)->tempcpu = item->valuedouble;
            //_ZK_PROTO_DEBUG("%f\r\n",((info_jsdpcpu *)cpuinfo_tmep)->tempcpu);

            _jsoninfo_get(&item, object, BMC_STATCPU, root);
                //return 3;
            ((info_jsdpcpu *)cpuinfo_tmep)->statcpu = item->valueint;
            //_ZK_PROTO_DEBUG("%d\r\n",((info_jsdpcpu *)cpuinfo_tmep)->statcpu);

            _jsoninfo_get(&item, object, BMC_UILCPU, root);
                //return 3;
            ((info_jsdpcpu *)cpuinfo_tmep)->uilcpu = item->valueint;
            //_ZK_PROTO_DEBUG("%d\r\n",((info_jsdpcpu *)cpuinfo_tmep)->uilcpu);

            _jsoninfo_get(&item, object, BMC_STATSTAT, root);
                //return 3;
            ((info_jsdpcpu *)cpuinfo_tmep)->statcpu = item->valueint;
            //_ZK_PROTO_DEBUG("%d\r\n",((info_jsdpcpu *)cpuinfo_tmep)->statcpu);

            _jsoninfo_get(&item, object, BMC_MEMCPU, root);
                //return 3;
            ((info_jsdpcpu *)cpuinfo_tmep)->memcpu = item->valueint;
            //_ZK_PROTO_DEBUG("%d\r\n",((info_jsdpcpu *)cpuinfo_tmep)->memcpu);
        }
        else if (mod_choice == MOD_SWITCH)
        {
            _jsoninfo_get(&item, object, BMC_TEMPMOD, root);
                //return 3;
            ((info_switchcpu *)cpuinfo_tmep)->tempmod = item->valuedouble;
            //_ZK_PROTO_DEBUG("%d\r\n",((info_switchcpu *)cpuinfo_tmep)->tempmod);

            _jsoninfo_get(&item, object, BMC_TEMPSWITCH, root);
                //return 3;

            ((info_switchcpu *)cpuinfo_tmep)->tempswitch = item->valuedouble;
            //_ZK_PROTO_DEBUG("%d\r\n",((info_switchcpu *)cpuinfo_tmep)->tempswitch);

            _jsoninfo_get(&item, object, BMC_STAOPT1, root);
                //return 3;
            ((info_switchcpu *)cpuinfo_tmep)->staopt1 = item->valueint;
            //_ZK_PROTO_DEBUG("%d\r\n",((info_switchcpu *)cpuinfo_tmep)->staopt1);

            _jsoninfo_get(&item, object, BMC_STAOPT2, root);
                //return 3;
            ((info_switchcpu *)cpuinfo_tmep)->staopt2 = item->valueint;
            //_ZK_PROTO_DEBUG("%d\r\n",((info_switchcpu *)cpuinfo_tmep)->staopt2);

						_jsoninfo_get(&item, object, BMC_NUMLINK, root);
						    //return 3;
						((info_switchcpu *)cpuinfo_tmep)->numlink = item->valueint;

            //新增端口状态解析
            _jsoninfo_get(&item, object, BMC_STALINK1, root);
                //return 3;
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[0]=(((item->valueint)&(1<<0))>>0);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[1]=(((item->valueint)&(1<<1))>>1);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[2]=(((item->valueint)&(1<<2))>>2);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[3]=(((item->valueint)&(1<<3))>>3);
            //_ZK_PROTO_DEBUG("%d\r\n",((info_switchcpu *)cpuinfo_tmep)->stalink1[0]);
            //_ZK_PROTO_DEBUG("%d\r\n",((info_switchcpu *)cpuinfo_tmep)->stalink1[1]);
            //_ZK_PROTO_DEBUG("%d\r\n",((info_switchcpu *)cpuinfo_tmep)->stalink1[2]);
            //_ZK_PROTO_DEBUG("%d\r\n",((info_switchcpu *)cpuinfo_tmep)->stalink1[3]);
						((info_switchcpu *)cpuinfo_tmep)->stalink1[4]=(((item->valueint)&(1<<4))>>4);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[5]=(((item->valueint)&(1<<5))>>5);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[6]=(((item->valueint)&(1<<6))>>6);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[7]=(((item->valueint)&(1<<7))>>7);
						((info_switchcpu *)cpuinfo_tmep)->stalink1[8]=(((item->valueint)&(1<<8))>>8);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[9]=(((item->valueint)&(1<<9))>>9);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[10]=(((item->valueint)&(1<<10))>>10);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[11]=(((item->valueint)&(1<<11))>>11);
						((info_switchcpu *)cpuinfo_tmep)->stalink1[12]=(((item->valueint)&(1<<12))>>12);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[13]=(((item->valueint)&(1<<13))>>13);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[14]=(((item->valueint)&(1<<14))>>14);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[15]=(((item->valueint)&(1<<15))>>15);
						((info_switchcpu *)cpuinfo_tmep)->stalink1[16]=(((item->valueint)&(1<<16))>>16);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[17]=(((item->valueint)&(1<<17))>>17);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[18]=(((item->valueint)&(1<<18))>>18);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[19]=(((item->valueint)&(1<<19))>>19);
						((info_switchcpu *)cpuinfo_tmep)->stalink1[20]=(((item->valueint)&(1<<20))>>20);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[21]=(((item->valueint)&(1<<21))>>21);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[22]=(((item->valueint)&(1<<22))>>22);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[23]=(((item->valueint)&(1<<23))>>23);
						((info_switchcpu *)cpuinfo_tmep)->stalink1[24]=(((item->valueint)&(1<<24))>>24);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[25]=(((item->valueint)&(1<<25))>>25);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[26]=(((item->valueint)&(1<<26))>>26);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[27]=(((item->valueint)&(1<<27))>>27);
						((info_switchcpu *)cpuinfo_tmep)->stalink1[28]=(((item->valueint)&(1<<28))>>28);
            ((info_switchcpu *)cpuinfo_tmep)->stalink1[29]=(((item->valueint)&(1<<29))>>29);

            _jsoninfo_get(&item,object,BMC_STALINK2,root);
                //return 3;
            ((info_switchcpu *)cpuinfo_tmep)->stalink2[0]=(((item->valueint)&(1<<0))>>0);
            ((info_switchcpu *)cpuinfo_tmep)->stalink2[1]=(((item->valueint)&(1<<1))>>1);
						((info_switchcpu *)cpuinfo_tmep)->stalink2[2]=(((item->valueint)&(1<<2))>>2);
            ((info_switchcpu *)cpuinfo_tmep)->stalink2[3]=(((item->valueint)&(1<<3))>>3);
					  ((info_switchcpu *)cpuinfo_tmep)->stalink2[4]=(((item->valueint)&(1<<4))>>4);
            ((info_switchcpu *)cpuinfo_tmep)->stalink2[5]=(((item->valueint)&(1<<5))>>5);
						((info_switchcpu *)cpuinfo_tmep)->stalink2[6]=(((item->valueint)&(1<<6))>>6);
            ((info_switchcpu *)cpuinfo_tmep)->stalink2[7]=(((item->valueint)&(1<<7))>>7);
					  ((info_switchcpu *)cpuinfo_tmep)->stalink2[8]=(((item->valueint)&(1<<8))>>8);
            ((info_switchcpu *)cpuinfo_tmep)->stalink2[9]=(((item->valueint)&(1<<9))>>9);
						((info_switchcpu *)cpuinfo_tmep)->stalink2[10]=(((item->valueint)&(1<<10))>>10);
            ((info_switchcpu *)cpuinfo_tmep)->stalink2[11]=(((item->valueint)&(1<<11))>>11);
						((info_switchcpu *)cpuinfo_tmep)->stalink2[12]=(((item->valueint)&(1<<12))>>12);
            ((info_switchcpu *)cpuinfo_tmep)->stalink2[13]=(((item->valueint)&(1<<13))>>13);
						((info_switchcpu *)cpuinfo_tmep)->stalink2[14]=(((item->valueint)&(1<<14))>>14);
            ((info_switchcpu *)cpuinfo_tmep)->stalink2[15]=(((item->valueint)&(1<<15))>>15);
						((info_switchcpu *)cpuinfo_tmep)->stalink2[16]=(((item->valueint)&(1<<16))>>16);
            ((info_switchcpu *)cpuinfo_tmep)->stalink2[17]=(((item->valueint)&(1<<17))>>17);
						((info_switchcpu *)cpuinfo_tmep)->stalink2[18]=(((item->valueint)&(1<<18))>>18);

            _jsoninfo_get(&item, object, BMC_SOTFVER, root);
                //return 3;
            strcpy(Msgsoftver.switch_soft, item->valuestring);
            //_ZK_PROTO_DEBUG("%s\r\n",Msgsoftver.switch_soft);
        }
        else if (mod_choice == MOD_POWER)
        {
            ;
        }
        else if (mod_choice == MOD_STG)
        {
            ;
        }

    }
    }

    cJSON_Delete(root);

    return 0;
}
