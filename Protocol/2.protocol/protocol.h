#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "main.h"

#define ZKPRO_HEAD		(0x7e)
#define ZKPRO_TAIL		(0xfd)

#define BMC_TEMPMOD     "TEMPMOD"
#define BMC_TEMPCPU		"TEMPCPU"
#define BMC_STATCPU 	"STATCPU"
#define BMC_UILCPU 		"UILCPU"
#define BMC_STATSTAT	"STASTAT"
#define BMC_MEMCPU		"MEMCPU"
#define BMC_VOL12		"VOL12"
#define BMC_VOLCPU		"VOLCPU"
#define BMC_VOL3V3  	"VOL3V3"

#define BMC_BATLEV		"BATLEV"
#define BMC_TIMSYNERR	"TIMSYNERR"
#define BMC_SOTFVER		"SOTFVER"
#define BMC_UILDISK		"UILDISK"//数据盘使用率
#define BMC_TCMPCPUERR	"TEMPCPUERR"//超温警告
#define BMC_DISKERR		"DISKERR"//数据盘满警告
#define BMC_BATLEVERR	"BATLEVERR"//电池电量警告

#define BMC_JSDP		"JSDP"
#define BMC_JSDP_1		"JSDP1"
#define BMC_JSDP_2		"JSDP2"
#define BMC_JSDP_3		"JSDP3"
#define BMC_VOLCPU1 	"VOLCPU1"
#define BMC_VOLCPU2		"VOLCPU2"
#define BMC_VOLBRG1     "VOLBRG1"
#define BMC_VOLBRG2     "VOLBRG2"
#define BMC_CURRENT     "CURRENT"
#define BMC_TEMPCPU1    "TEMPCPU1"
#define BMC_TEMPCPU2    "TEMPCPU2"
#define BMC_STATCPU1    "STATCPU1"
#define BMC_STATCPU2    "STATCPU2"
#define BMC_UILCPU1     "UILCPU1"
#define BMC_UILCPU2     "UILCPU2"
#define BMC_STASTAT1    "STASTAT1"
#define BMC_STASTAT2    "STASTAT2"
#define BMC_MEMCPU1     "MEMCPU1"
#define BMC_MEMCPU2     "MEMCPU2"
//新增信息
#define	BMC_BATLEV1		"BATLEV1"
#define	BMC_BATLEV2		"BATLEV2"
#define BMC_TIMSYNERR1	"TIMSYNERR1"
#define BMC_TIMSYNERR2	"TIMSYNERR2"
#define BMC_SOFTVER1	"SOFTVER1"
#define BMC_SOFTVER2	"SOFTVER2"
#define BMC_UILDISK1	"UILDISK1"
#define BMC_UILDISK2	"UILDISK2"
#define BMC_TEMPCPUERR1	"TEMPCPUERR1"
#define BMC_TEMPCPUERR2	"TEMPCPUERR2"
#define BMC_DISKERR1	"DISKERR1"
#define BMC_DISKERR2	"DISKERR2"
#define BMC_BATLEVERR1	"BATLEVERR1"
#define BMC_BATLEVERR2	"BATLEVERR2"


#define BMC_CCDP		"CCDP"
#define BMC_TEMPMOD		"TEMPMOD"
#define BMC_TEMPCPU		"TEMPCPU"
#define BMC_TEMPCPUERR	"TEMPCPUERR"
#define BMC_VOL12		"VOL12"
#define BMC_VOLCPU		"VOLCPU"
#define BMC_VOLMEM		"VOLMEM"
#define BMC_STACPU		"STACPU"
#define BMC_UILCPU		"UILCPU"
#define BMC_MEMCPU		"MEMCPU"		
#define BMC_STABIG		"STABIG"
#define BMC_MEMBIG		"MEMBIG"
#define BMC_ULIBIG		"ULIBIG"
#define BMC_ULIBIGERR	"ULIBIGERR"
#define BMC_STASMAL		"STASMAL"
#define BMC_MEMSMAL		"MEMSMAL"
#define BMC_ULISMAL		"ULISMAL"
#define BMC_ULISMALERR  "ULISMALERR"
#define	BMC_BATLEV		"BATLEV"
#define BMC_BATLEVERR	"BATLEVERR"
#define BMC_STAOPT		"STAOPT"
#define BMC_TIMSYNERR	"TIMSYNERR"
#define BMC_SOFTVER		"SOFTVER"
#define BMC_CCMOD 		"CCMOD"
#define BMC_RAID		"RAID"
#define BMC_NAS1		"NAS1"
#define BMC_NAS2		"NAS2"
#define BMC_NAS3		"NAS3"
#define BMC_NAS4		"NAS4"
#define BMC_NAS5		"NAS5"
#define BMC_NAS6		"NAS6"
#define BMC_NAS7		"NAS7"
#define BMC_NAS8		"NAS8"


#define BMC_DYDP		"DYDP"
#define BMC_STA270V1	"STA270V1"
#define BMC_STA270V2    "STA270V2"
#define BMC_STA28V      "STA28V"
#define BMC_CURR270V    "CURR270V"
#define BMC_CURR28V 	"CURR28V"
#define BMC_TEMPMOD 	"TEMPMOD"
#define BMC_DYMOD		"DYMOD"
#define BMC_SOFTVER		"SOFTVER"


#define	BMC_SWITCH		"JHDP" 
#define BMC_TEMPMOD  	"TEMPMOD"
#define BMC_TEMPSWITCH	"TEMPSWITCH"
#define BMC_STAOPT1		"STAOPT1"
#define BMC_STAOPT2		"STAOPT2"
#define BMC_NUMLINK		"NUMLINK"
#define BMC_STALINK1  "STALINK1"//千兆端口
#define BMC_STALINK2  "STALINK2"//万兆端口
#define BMC_SOFTVER		"SOFTVER"//软件版本


#define SOFTVER_LEN  50


typedef enum
{
	//分发给任务处理机的命令
	//CMD_GET_STAT2		 =0x0201,			//配电分机通信查询
	CMD_SET_STAWORK 	=0x0202, 		//整机工作状态
	CMD_GET_STAT1		 =0x0101,			//显控通信查询
	CMD_GET_HARDWARE =0x0102, 		//硬件查询
	CMD_SET_STAKVM	 =0x0103,			//KVM切换报文
	CMD_SET_STANUM	 =0x0104,			//初始化参数配置
	CMD_GET_SOFTVER  =0x0105,   	//软件版本信息查询
	CMD_SET_STACONTROL =0x0106,  	//模块上电下电复位
	CMD_SET_DISKENABLE =0x0107, 	//数据盘销毁使能
	CMD_SET_STADISK =0x0108, 			//数据盘销毁状态查询
	CMD_SET_STADISKNOTE=0x0109, 	//数据盘销毁记录查询
	CMD_SET_TAIN=0x010A, 					//维护查询命令
	//CMD_SET_STAWORK =0x010b,		//整机工作状态（新协议规定）
	//任务计算机上报给显控的报文
	CMD_UP_STAT1 =0x0281,				//配电分机命令应答
	CMD_UP_STAT	 =0x0181,				//心跳命令应答
	CMD_UP_STATHEALTHY	=0x0182,	//自主上报健康状态
	CMD_UP_SOFTVER  =0x0183,   	//软件版本信息上报查询
	CMD_UP_HARDWARE =0x0184,    //硬件设备信息查询
	CMD_UP_STADISK =0x0185,			//数据盘销毁上报
	CMD_UP_STADISKNOTE=0x0186,	//数据盘销毁记录上报
	
}cmd_TCP;
typedef struct
{
	uint16_t IP_src;//源地址
	uint16_t IP_dst;//目的地址
	uint16_t cmdname;//命令号
	uint16_t ediname;//版本号
	uint8_t attribute;//属性
	uint8_t seq;//报文序号
	uint16_t datanum;//总包数
	uint16_t dataname;//当前包号
	uint16_t chek;//校验码
	uint16_t datecmd;// 控制命令
	cmd_TCP cmd;
	cmd_TCP reply_cmd;
}info_TCP;






typedef enum
{
	//下行(查询)不带载荷数据，上报需要载荷数据
	CMD_GET_STAT		= 0x1,	//健康状态获取
	
	   
	//下行(配置)不带载荷数据，不需要上报
	CMD_SET_RESETCPU1 =0x80,    //复位CPU1
	CMD_SET_RESETCPU2 =0x81,    //复位CPU2
	CMD_SET_POWDOWN1=0x82, 	    //模块下电(关闭12V电源)
	CMD_SET_POWDOWN2=0x83, 	    //模块下电(关闭12V电源)
	CMD_SET_POWUP1	  =0x84,    //模块上电(开启12V电源)
	CMD_SET_POWUP2	  =0x85,    //模块上电(开启12V电源)
	CMD_GET_DEST_ENABLE = 0x86, //销毁使能控制
	CMD_GET_DEST=0x87,                 //销毁状态查询
	CMD_GET_DEST_RECORD=0x88,               //销毁记录查询
	CMD_GET_INIT_CFG=0x89,             //初始化配置
	CMD_GET_SOFT_VERS=0x90,            //软件版本信息
	CMD_GET_HARD_VERS=0x91,            //硬件版本信息

}cmd_RS485;

typedef enum
{
	COMM_CMD_GET_STAT		= 0x1,	//健康状态
	   
	//下行(配置)不带载荷数据，不需要上报
	COMM_CMD_SET_RESETCPU=0x80,    //复位CPU
	COMM_CMD_SET_POWERDOWN=0x81,    //电源
}cmd_CommCpu;

typedef enum
{
	MOD_CPU1,     //其他模块上报
	MOD_CPU2,      //其他模块查询
}choice_cpu;

typedef enum
{
	MOD_SWITCH=0Xeb,					//交换刀片
	MOD_JSDP_1=0x3D,					//计算刀片1
	MOD_JSDP_2=0X1C,					//计算刀片2
	MOD_JSDP_3=0X3B,					//计算刀片3
	MOD_STG=0X3E,					    //存储刀片
	MOD_POWER=0Xae,				  	//电源刀片
}mod_Type;

typedef struct
{
	uint8_t src;
	uint8_t dst;
	cmd_RS485 cmd;
}info_rs485;

typedef struct
	
{	
	uint16_t statcpu;
	int16_t tempcpu;
	uint16_t uilcpu;
	uint16_t stasta;
	uint16_t memcpu;
	uint16_t Vbatlev;//电池电量
	uint16_t timsynerr;//时间同步误差
	int16_t tempcpuerr;//温度告警
	uint16_t diskerr;//磁盘满告警
	uint16_t batleverr;//电池电量告警
	uint16_t uildisk;//磁盘使用率
}info_jsdpcpu;

typedef struct
{
	int16_t tempmod;
	int16_t tempswitch;
	uint16_t staopt1;
	uint16_t staopt2;
	uint16_t numlink;
	uint16_t stalink1[30];//千兆端口
	uint16_t stalink2[19];//万兆端口
	
}info_switchcpu;

typedef struct
{	uint16_t modID;//模块编号
	uint16_t current;
	uint16_t vol12v;
	uint16_t vol3v3;
	uint16_t volcpu1;
	uint16_t volcpu2;
	uint16_t volbrg1;
	uint16_t volbrg2;
	int16_t tempmod;
	info_jsdpcpu cpuinfo[2];
}info_jsdpmod;//向显控发送得计算刀片的数据

typedef struct
{	uint16_t modID;
	uint16_t modsat;
	uint16_t vol12v;
	uint16_t volswitch;
	info_switchcpu switchcpuinfo;
}info_switchmod; //向显控发送得交换刀片数据

typedef struct
{	
	uint16_t modID;
	uint16_t dymod;
	uint16_t sta270v1;
	uint16_t sta270v2;
	uint16_t sta28v;
	uint16_t current270v;
	uint16_t current28v;
	int16_t tempmod;
}info_powermod;//向显控发送的电源刀片的数据

typedef struct
{	
	uint16_t modID;
	int16_t tempcpu;
	int16_t tempmod;
	uint16_t vol12v;
	uint16_t volcpu;
	uint16_t modstat;
	uint16_t stacpu;
	int16_t tmpwaring;
	uint16_t ddrvol;
	uint16_t cpuusing;
	uint16_t cpumemusing;
	uint16_t largessdh;
	uint16_t largessdc;
	uint16_t largssdu;
	uint16_t largessdw;
	uint16_t samallssdh;
	uint16_t samallssdc;
	uint16_t samallssdu;
	uint16_t samallssdw;
	uint16_t raidmode;
	uint16_t rtcpower;
	uint16_t rtcpowerw;
	uint16_t lightstatus;
	uint16_t timesynerror;
	uint16_t ssd1status;
	uint16_t ssd2status;
	uint16_t ssd3status;
	uint16_t ssd4status;
	uint16_t ssd5status;
	uint16_t ssd6status;
	uint16_t ssd7status;
	uint16_t ssd8status;
}info_csdpmod;//向显控发送的存储刀片的数据



typedef struct
{	
	char 	jsdp1_soft[SOFTVER_LEN];
	char 	jsdp2_soft[SOFTVER_LEN];
	char 	jsdp3_soft[SOFTVER_LEN];
	char 	csdp_soft[SOFTVER_LEN];
	char 	power_soft[SOFTVER_LEN];
	char  switch_soft[SOFTVER_LEN];
	char  bmc_soft[SOFTVER_LEN];
	
}info_softver;//软件版本上报

typedef struct
{	
	uint16_t modID;
	uint16_t stafengshan1;//风扇状态
	uint16_t stafengshan2;
	uint16_t stafengshan3;
	uint16_t fengshanspeed;//转速
	uint16_t fengshanspeeddate;//转速值 rpm
	uint16_t reserve[2];//预留
	
}info_fengshan;//向显控发送的风扇模块的数据

typedef struct
{
	info_jsdpmod   bmc_jsdp_1;
	info_jsdpmod   bmc_jsdp_2;
	info_jsdpmod   bmc_jsdp_3;
	info_switchmod bmc_switch;
	info_csdpmod   bmc_csdp;
	info_powermod  bmc_power;
	info_fengshan	 bmc_fengshan;
}info_bmc;//要发送的数据包

//*******************顺序和协议一致的结构体***********************************************
typedef struct
{	
	uint16_t modID;//模块编号
	uint16_t current;
	uint16_t statcpu1;
	uint16_t statcpu2;
	int16_t tempcpu1;
	int16_t tempcpu2;
	int16_t tempmod;
	int16_t tempcpuerr;//温度告警
	uint16_t vol12v;
	uint16_t volcpu1;
	uint16_t volcpu2;
	uint16_t volbrg1;
	uint16_t volbrg2;
	uint16_t uilcpu1;//cpu1使用率
	uint16_t uilcpu2;
	uint16_t memcpu1;
	uint16_t memcpu2;
	uint16_t stasta1;//cpu1盘状态
	uint16_t disk1;			//磁盘容量
	uint16_t uildisk1;//磁盘1使用率
	uint16_t diskerr1;//cpu1磁盘满告警
	uint16_t stasta2;//cpu2盘状态
	uint16_t disk2;
	uint16_t uildisk2;
	uint16_t diskerr2;//cpu2磁盘满告警
	uint16_t Vbatlev;//电池电量
	uint16_t batleverr;//电池电量告警
	uint16_t timsynerr;//时间同步误差
	uint16_t reserve[2];//预留
}info_jsdpmod1;



typedef struct
{
	uint16_t modID;
	uint16_t modsat;//模块状态
	int16_t tempmod;
	int16_t tempswitch;
	int16_t tempcpuerr;//超温警告
	uint16_t vol12v;
	uint16_t volswitch;
	uint16_t staopt;//链路光功率检测
	uint16_t numlink; //端口数量
	uint16_t stalink[64];//端口
	uint16_t reserve[2];//预留
	
}info_switchmod1;


typedef struct
{
	uint16_t modID;
	uint16_t modsat;//模块状态
	uint16_t stacpu;
	int16_t tempmod;
	int16_t tempcpu;
	int16_t tempwaring;//温度警告
	uint16_t vol12v;
	uint16_t volcpu;
	uint16_t volmem;
	uint16_t uilcpu;//CPU使用率
	uint16_t memcpu;//内存使用率
	uint16_t stabig;//大容量存储状态
	uint16_t stalage;//大容量存储容量
	uint16_t staulage;//大容量使用率
	uint16_t stawarlage; //大容量存储警告
	uint16_t stasamlstatu; //小容量状态
	uint16_t stasamllage;//小容量存储
	uint16_t stasamlusing; //小容量使用
	uint16_t stasamwarlage; //小容量警告
	uint16_t raid;
	uint16_t batlev;//电池电量
	uint16_t batlevwar;//电池电量警告
	uint16_t statuopt;//光状态
	uint16_t timsynerr;//时间同步误差
	uint16_t nas1;
	uint16_t nas2;
	uint16_t nas3;
	uint16_t nas4;
	uint16_t nas5;
	uint16_t nas6;
	uint16_t nas7;
	uint16_t nas8;
	uint16_t reserve[2];//预留
}info_csdpmod1;//向显控发送的存储刀片的数据

typedef struct
{	uint16_t modID;
	uint16_t dymod;
	uint16_t sta270v1;
	uint16_t sta270v2;
	uint16_t sta28v;
	uint16_t current270v;
	uint16_t current28v;
	int16_t tempmod;
	uint16_t reserve[2];//预留
}info_powermod1;//向显控发送的电源刀片的数据

typedef struct
{
	info_jsdpmod1    bmc_jsdp_1;
	info_jsdpmod1    bmc_jsdp_2;
	info_jsdpmod1    bmc_jsdp_3;
	info_switchmod1 bmc_switch;
	info_csdpmod1   bmc_csdp;
	info_powermod1  bmc_power;
	info_fengshan	  bmc_fengshan;
}info_bmc_new;//要发送的数据包




typedef struct
{	
	uint32_t unAppID; 		//软件ID		
	char cAppName[64];	//软件名称
	uint8_t ucAppType;			//软件类型		
	uint32_t unAppVersion; //软件版本号		
	char cReleaseTime[7];		//版本发布时间
	char cInstallTime[7];		//软件安装时间
	uint8_t ucDirDepth;    //备份
}info_appVer;


typedef struct
{	uint16_t usPackNumber; 		//包序号
	uint16_t usFinishFlag;		//结束标志
	uint16_t usLength;				//数据长度
	info_appVer AppVerInfo_jsdp_1;			//应用软件产品规格信息数据
	info_appVer AppVerInfo_jsdp_2;
	info_appVer AppVerInfo_jsdp_3;
	info_appVer AppVerInfo_switch;
	info_appVer AppVerInfo_csdp;
	info_appVer AppVerInfo_power;
	
}info_up_softver;					//软件版本上报


typedef struct
{	
	char chassisID[10]; 				//机箱编号
	char manufactor[10]; 				//厂家
	char date[8];       				//出厂日期
	unsigned long powertime;		//上电总时间（小时）
	char reserve[10]; 					//预留
	uint16_t modnum;						//模块个数		
}info_up_device;//硬件设备信息上报






//*******************顺序和协议一致的结构体******************************************




#define UART_MAX_LEN		(512)
#define JSONAME_MAX_LEN		(16)

#define PRO_OFFSET_HEADF	(0)
#define PRO_OFFSET_HEADS	(1)
#define PRO_OFFSET_SRCID	(2)
#define PRO_OFFSET_DSTID	(3)
#define PRO_OFFSET_CMD		(4)
#define PRO_OFFSET_MSG		(5)
#define PRO_OFFSET_TAILF	(6)
#define PRO_OFFSET_TAILS	(7)

#define RS485_PRO_LEN	(7) //2byte(head)+2byte(tail)+1
							//byte(srcid)+1byte(desid)+
							//1byte(cmd)=7byte

#define PRO_COMM_OFFSET_HEADF	(0)
#define PRO_COMM_OFFSET_HEADS	(1)
#define PRO_COMM_OFFSET_SRCID	(2)
#define PRO_COMM_OFFSET_MSG     (3)
#define PRO_COMM_OFFSET_TAILF	(4)
#define PRO_COMM_OFFSET_TAILS	(5)

#define COMM_PRO_LEN		(5) //2byte(head)+2byte(tail)+1
								//byte(srcid)=5byte
/**
 * @brief 数据发送接口
 *
 */
typedef uint16_t(*protocol_send_interface)(uint8_t *, uint16_t);
/**
 * @brief 数据读取接口
 * 
 */
typedef uint16_t(*protocol_recv_interface)(uint8_t *);



//协议收发完全结构体
typedef struct
{
	info_rs485 rs485_info;
    info_bmc bmc_msg;
    protocol_send_interface send_485;//协议发送接口
    protocol_recv_interface recv_485;//协议读取接口
	protocol_send_interface send_cpu1;
	protocol_send_interface send_cpu2;
	protocol_recv_interface recv_cpu1;
	protocol_recv_interface recv_cpu2;
} protocol_t;


void send_healthy();
uint32_t tcp_MsgpackSend(uint32_t TCPsocket, uint8_t *data,int pack_len,info_TCP info);
int32_t tcp_MsgpackRecv(uint32_t tcpsocket);
void server_contol(void *pvParameters);



uint8_t Rs485_MsgPackSend(info_bmc bmcinfo2,info_rs485 info);
uint8_t Rs485_MsgPackRecv(info_bmc *bmcinfo,u8 *recvdata);
void bmc_infoinit(void);
uint8_t rs485_task_func(u8 *recvdata);
uint8_t CommCpu_MsgPackSend(cmd_CommCpu cmd,choice_cpu cpu_choice);
uint8_t CommCpu_MsgPackRecv(choice_cpu cpu_choice,mod_Type mod_choice);
void rs485_send(cmd_RS485 RS485_CMD,mod_Type mod_choice);


#endif


