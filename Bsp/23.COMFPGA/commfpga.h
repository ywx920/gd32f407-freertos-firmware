#ifndef _COMMFPGA_H_
#define _COMMFPGA_H_

#include "sys_config.h"



uint16_t send_fpgamsg(uint8_t *buf, uint16_t mstrlen);
void SendTemp(void);
void SetFpgaon(void);
void SetFpgaoff(void);
void SetFpgaReset(void);

#endif //_COMMFPGA_H_
