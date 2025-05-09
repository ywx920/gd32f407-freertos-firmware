
#include "commfpga.h"


uint8_t temperature_data[2] = {0x4A, 0x40};  // 温度数据为 0x4A40

/* 十进制转十六进制函数 */
void decToHex(int dec, char* hex) {
    int i = 0;
    
    // 将十进制数除以16，直到商为0，同时将余数转换为十六进制字符
    while (dec != 0) {
        int remainder = dec % 16;
        if (remainder < 10) {
            hex[i] = remainder + '0';
        } else {
            hex[i] = remainder + 'A' - 10;
        }
        dec /= 16;
        i++;
    }
    // 反转转换后的字符串
    int len = strlen(hex);
    for (int j = 0; j < len / 2; j++) {
        char temp = hex[j];
        hex[j] = hex[len - 1 - j];
        hex[len - 1 - j] = temp;
    }
    hex[i] = '\0';  // 字符串末尾添加结束符
}

void temp_get()
{

    uint16_t value = 64 * get_nst175_temperature();
    // 拆分成两个字节
    uint8_t high_byte = (value >> 8) & 0xFF; // 高字节
    uint8_t low_byte = value & 0xFF; // 低字节
    
    // 存储到数组中
    temperature_data[0] = low_byte; // 存储高字节
    temperature_data[1] = high_byte; // 存储低字节

    // printf("1111111 %s\r\n", temperature_data);
    // printf("2222222= %d: %d\r\n", high_byte, low_byte);
    
}

uint16_t send_fpgamsg(uint8_t *buf, uint16_t mstrlen)
{
	send_string(buf, mstrlen, USART2);
	return mstrlen;
}

void SendTemp(void)
{
	uint8_t DATA1[11] = {0xA5, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x03, 0x00, 0xFF, 0xFF, 0x5A};
    temp_get();
    temppritf();
	DATA1[8] = temperature_data[1];
	DATA1[9] = temperature_data[0];
	printf("sendtemp vaule to FPGA: %d\r\n", sizeof(DATA1));
    printf("DATA1: ");
    for (int i = 0; i < 11; i++) {
        printf("%02X ", DATA1[i]);
    }
    printf("\n");

	send_fpgamsg(DATA1, sizeof(DATA1));
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
sendtemp, SendTemp, sendtemp vaule to FPGA);


void SetFpgaon(void)
{
	uint8_t DATA2[11] = {0xA5, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x03, 0x01, 0x00, 0x00, 0x5A};
	printf("set FPGA poweron %d\r\n", sizeof(DATA2));
    printf("DATA2: ");
    for (int i = 0; i < 11; i++) {
        printf("%02X ", DATA2[i]);
    }
    printf("\n");
	send_fpgamsg(DATA2, sizeof(DATA2));
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
poweron, SetFpgaon, set FPGA poweron);

void SetFpgaoff(void)
{
	uint8_t DATA3[11] = {0xA5, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x03, 0x01, 0x00, 0x01, 0x5A};
	printf("set FPGA poweroff %d\r\n", sizeof(DATA3));
    printf("DATA3: ");
    for (int i = 0; i < 11; i++) {
        printf("%02X ", DATA3[i]);
    }
    printf("\n");
	send_fpgamsg(DATA3, sizeof(DATA3));
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
poweroff, SetFpgaoff, set FPGA poweroff);

void SetFpgaReset(void)
{
	uint8_t DATA4[11] = {0xA5, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x03, 0x02, 0x00, 0x01, 0x5A};
	printf("set FPGA reset %d\r\n", sizeof(DATA4));
    printf("DATA4: ");
    for (int i = 0; i < 11; i++) {
        printf("%02X ", DATA4[i]);
    }
    printf("\n");
	send_fpgamsg(DATA4, sizeof(DATA4));
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
reset, SetFpgaReset, set FPGA reset);