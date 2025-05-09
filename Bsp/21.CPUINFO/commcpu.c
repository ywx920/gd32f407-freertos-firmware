#include "main.h"
#include "stm32f4xx_hal.h"


#define COMM_SERVER_ID 0x8000  //cpu主机
#define COMM_CLIENT_ID 0x7e7e   //板卡

#define REQ_CMD_TYPE_CPU	0	请求获取cpu信息
#define REQ_CMD_TYPE_MEM	1	请求获取内存信息
#define REQ_CMD_TYPE_DISK	2	请求获取磁盘信息
#define REQ_CMD_TYPE_NET	3	请求获取网络信息

#define BMC_RESP_CMD_PERIOD	0	周期上报主机信息
#define BMC_RESP_CMD_CPU	1	上报cpu信息
#define BMC_RESP_CMD_MEM	2	上报内存信息
#define BMC_RESP_CMD_DISK	3	上报磁盘信息
#define BMC_RESP_CMD_NET	4	上报网络信息




typedef struct {
    uint16_t REQ_MSG;
    uint16_t REPLY_MSG;
} MessageFrame;


// 定义通信帧结构体
typedef struct {
    uint32_t frameID;
    uint16_t srcID;
    uint16_t dstID;
    uint16_t length;
    uint16_t cmdID;
    uint32_t crc;
} CommunicationFrame;

UART_HandleTypeDef huart2;

// 函数原型声明
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

// 主函数
int main(void) {
    // 初始化HAL库
    HAL_Init();

    // 配置系统时钟
    SystemClock_Config();

    // 初始化所有的硬件
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    // 启动串口接收中断
    HAL_UART_Receive_IT(&huart2, (uint8_t *)buffer, 1);

    // 构造通信帧
    CommunicationFrame frame;
    frame.frameID = 0x12345678; // 帧ID
    frame.srcID = 0x01;         // 源设备ID
    frame.dstID = 0x02;         // 目的设备ID
    frame.length = 0x0004;      // 帧长
    frame.cmdID = 0x0003;       // 指令码
    // 计算CRC，这里假设CRC为0
    frame.crc = 0x00000000;

    // 发送通信帧
    HAL_UART_Transmit(&huart2, (uint8_t *)&frame, sizeof(frame), HAL_MAX_DELAY);

    while (1) {
        // 主循环
    }
}

// 串口接收中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    // 在这里处理接收到的数据
    // 例如，可以将接收到的数据存储在一个缓冲区中，并在需要时进行处理
}

// USART2 初始化函数
static void MX_USART2_UART_Init(void) {
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 9600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
}
