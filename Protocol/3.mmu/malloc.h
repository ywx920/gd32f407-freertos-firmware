/**
  ******************************************************************************
  * @file           : malloc.h
  * @brief          : Header for malloc.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 hopewellgroup.
  * All rights reserved.
  *
  * This software is licensed by hopewellgroup under Ultimate Liberty license
  *
  ******************************************************************************
  */

#ifndef __MALLOC_H
#define __MALLOC_H

#include "stdint.h"

#ifndef NULL
#define NULL 0
#endif

#define MEM_BLOCK_SIZE              256     /* 内存块大小 */
#define MEM_MAX_SIZE                30*1024 /* 最大管理30k */
#define MEM_ALLOC_TABLE_SIZE    MEM_MAX_SIZE/MEM_BLOCK_SIZE/* 内存表大小 */

/* 内存管理 */
struct _m_mallco_dev
{
    void (*init)(void);    /* 初始化           */
    uint8_t   (*perused)(void); /* 内存使用率       */
    uint8_t    *membase;        /* 内存池           */
    uint16_t   *memmap;         /* 内存管理状态表   */
    uint8_t     memrdy;         /* 内存管理是否就绪 */
};

/* 定义在 malloc.c*/
extern struct _m_mallco_dev mallco_dev;

/**************************** 用户使用函数 ************************************/
/**************************** 内存释放*/
void myfree(void *ptr);
/**************************** 内存分配*/
void *mymalloc(uint32_t size);
/**************************** 内存重新分配*/
void *myrealloc(void *ptr, uint32_t size);
/**************************** 复制内存*/
void mymemcpy(void *des, void *src, uint32_t n);
/**************************** 用户使用函数 ************************************/

#endif

/************************ (C) COPYRIGHT Hopewellgroup ***********END OF FILE***/
