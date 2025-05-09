#include "bsp_iicBus.h"
#include "bsp_tmp100.h"
#include "bsp_nct210.h"
#include "bsp_i2c.h"
#include "bsp_Control_xc6s.h"
#include "ipmb.h"

//#-------------------------------------------------------------------
static uint16_t g_t210temp = 0;
static uint16_t g_t100temp = 0;
//static uint16_t tmpOld = 0;
/*=========0=========1=========2=========3=========4=========5=========*/
/*
    卡尔曼滤波算法
*/
typedef struct 
{
    float Last_P;//上次估算协方差 不可以为0 ! ! ! ! ! 
    float Now_P;//当前估算协方差
    float out;//卡尔曼滤波器输出
    float Kg;//卡尔曼增益
    float Q;//过程噪声协方差
    float R;//观测噪声协方差
}Kalman;
void Kalman_Init(Kalman *kfp)
{
	kfp->Last_P = 1;			
	kfp->Now_P = 0;		
	kfp->out = 0;			
	kfp->Kg = 0;		
	kfp->Q = 0;
	kfp->R = 0.01;
}
float KalmanFilter(Kalman *kfp,float input)
{
   //预测协方差方程：k时刻系统估算协方差 = k-1时刻的系统协方差 + 过程噪声协方差
   kfp->Now_P = kfp->Last_P + kfp->Q;
   //卡尔曼增益方程：卡尔曼增益 = k时刻系统估算协方差 / （k时刻系统估算协方差 + 观测噪声协方差）
   kfp->Kg = kfp->Now_P / (kfp->Now_P + kfp->R);
   //更新最优值方程：k时刻状态变量的最优值 = 状态变量的预测值 + 卡尔曼增益 * （测量值 - 状态变量的预测值）
   kfp->out = kfp->out + kfp->Kg * (input -kfp->out);//因为这一次的预测值就是上一次的输出值
   //更新协方差方程: 本次的系统协方差付给 kfp->LastP 威下一次运算准备。
   kfp->Last_P = (1-kfp->Kg) * kfp->Now_P;
   return kfp->out;
}
static Kalman kfp_t210;
static Kalman kfp_tmp100;
/*=========0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========10=========11=========*/

uint16_t get_t210temp(void)
{
    return g_t210temp;
}

uint16_t get_t100temp(void)
{
    return g_t100temp;
}

void IICBUSTask(void* param)
{
    //uint8_t fanRunFlag = 0;
    
    vTaskDelay(3000);/* I2C 总线 在 任务启动后延迟2秒，若不延迟会异常 */
    //温度传感器初始化
    //tmp100_init();
    vTaskDelay(100);
    //nct210_init();
    vTaskDelay(100);
    
    //xc6s fpga 串口初始化 与 风扇板 通信使用
    vTaskDelay(1000);
    Kalman_Init(&kfp_t210);//卡尔曼滤波
    Kalman_Init(&kfp_tmp100);
    uart3_init(115200);
    uart3Send_char(100);//设置默认初始化 风扇转速
    //uint8_t test_num = 0;
    
    while(1)
    {
//        g_t210temp = get_nct210_temperature();
//        g_t210temp = KalmanFilter(&kfp_t210,g_t210temp)+3;//使用卡尔曼滤波时，尾部增加一个系数
//        vTaskDelay(100);
//        
//        g_t100temp = get_tmp100_temperature();
//        g_t100temp = KalmanFilter(&kfp_tmp100,g_t100temp)+3;//使用卡尔曼滤波时，尾部增加一个系数
//        vTaskDelay(900);
//        
//        /* ----------------------- */
//        fanRunFlag = getFanMode();//得到风扇运行模式
//        if(fanRunFlag == 0)//自动模式
//        {
//            if(tmpOld != g_t100temp)//当两次温度相同时，不执行逻辑
//            {
//                tmpOld = g_t100temp;
//                
//                if(g_t100temp < 45)
//                {
//                    uart3Send_char(60);
//                }
//                else
//                if((g_t100temp >= 45) && (g_t100temp < 55))
//                {
//                    uart3Send_char(70);
//                }
//                else
//                if((g_t100temp >= 55) && (g_t100temp < 65))
//                {
//                    uart3Send_char(80);
//                }
//                else
//                if((g_t100temp >= 65) && (g_t100temp < 75))
//                {
//                    uart3Send_char(90);
//                }
//                else 
//                if(g_t100temp > 75)
//                {
//                    uart3Send_char(98);
//                }
//            }
//        }
        
////        printf("g_t210temp:%d\r\n",g_t210temp);
////        vTaskDelay(100);
////        printf("g_t100temp:%d\r\n",g_t100temp);
        
        vTaskDelay(1000);

    }
}

