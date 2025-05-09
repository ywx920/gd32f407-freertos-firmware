#ifndef _MYSTRING_H_
#define _MYSTRING_H_

#include "main.h"

uint8_t my_strcmp(uint8_t *dest,uint8_t *source);
uint8_t findStr(char * func,char str);
uint8_t findstrnum(char * func,char str);
void findfuncname(char * str,char *str1);
int strToint(char *s,uint8_t num);
uint8_t findStrS(char * func,uint8_t num,char str);

#endif //_MYSTRING_H_
