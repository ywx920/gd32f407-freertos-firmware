#ifndef _RS485_H
#define _RS485_H

#define RS485_MAXBUFF_SIZE   512

#define RS485_PRO_HEAD       0x7e7e
#define RS485_PRO_TAIL       0xfdfd

#define RS485_PRO_HEAD_LEN   2
#define RS485_PRO_TAIL_LEN   2

void RS485_IrqHandler(void);
uint16_t send_485msg(uint8_t *buf,uint16_t mstrlen);
uint16_t recv_485msg(uint8_t *buf);
void RS485Task(void* param);

#endif