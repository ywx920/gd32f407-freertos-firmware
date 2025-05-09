/**
  ******************************************************************************
  * @file           : malloc.c
  * @brief          : Custome Main program body
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

#include "malloc.h"

/* 内存池(4字节对齐) */
/* 内部SRAM内存池 */
__align(4) uint8_t membase[MEM_MAX_SIZE];
/* 内存管理表 */
/* 内部SRAM内存池MAP */
uint16_t memmapbase[MEM_ALLOC_TABLE_SIZE];
/* 内存管理参数 */
/* 内存表大小 */
const uint32_t memtblsize = MEM_ALLOC_TABLE_SIZE;
/* 内存分块大小 */
const uint32_t memblksize = MEM_BLOCK_SIZE;
/* 内存总大小 */
const uint32_t memsize    = MEM_MAX_SIZE;

/* 设置内存 */
static void mymemset(void *s, uint8_t c, uint32_t count);
/* 内存管理 */
static void mem_init(void);
/* 内存分配 */
static uint32_t mem_malloc(uint32_t size);
/* 内存释放 */
static uint8_t mem_free(uint32_t offset);
/* 获得内存使用率 */
static uint8_t mem_perused(void);


/* 内存管理控制器 */
struct _m_mallco_dev mallco_dev =
{
    mem_init,                /* 内存初始化     */
    mem_perused,             /* 内存使用率     */
    membase,                 /* 内存池         */
    memmapbase,              /* 内存管理状态表 */
    0,                       /* 内存管理未就绪 */
};


/*******************************************************************************
函数名称：mymemcpy()
功    能：内存拷贝
说    明：
参    数: des，目的地址，src,源地址, n，拷贝字节长度
返    回：
注    意：
*******************************************************************************/
void mymemcpy(void *des, void *src, uint32_t n)
{
    uint8_t *xdes = des;
    uint8_t *xsrc = src;

    while (n--)
    {
        *xdes++ = *xsrc++;
    }
}

/*******************************************************************************
函数名称：mymemset()
功    能：内存设值
说    明：
参    数: s，内存首地址，c, 设置的值, count，设置的字节长度
返    回：
注    意：
*******************************************************************************/
void mymemset(void *s, uint8_t c, uint32_t count)
{
    uint8_t *xs = s;

    while (count--)
    {
        *xs++ = c;
    }
}

/*******************************************************************************
函数名称：mem_init()
功    能：内存管理初始化
说    明：
参    数:
返    回：
注    意：
*******************************************************************************/
void mem_init(void)
{
    /* 内存状态表数据清零 */
    mymemset(mallco_dev.memmap, 0, memtblsize * 2);
    /* 内存池所有数据清零 */
    mymemset(mallco_dev.membase, 0, memsize);
    /* 内存管理初始化OK  */
    mallco_dev.memrdy = 1;
}

/*******************************************************************************
函数名称：mem_perused()
功    能：内存使用率
说    明：
参    数:
返    回：使用率(0~100)
注    意：
*******************************************************************************/
uint8_t mem_perused(void)
{
    uint32_t used = 0;
    uint32_t i    = 0;

    for (i = 0; i < memtblsize; i++)
    {
        if (mallco_dev.memmap[i])
        {
            used++;
        }
    }

    return (used * 100) / (memtblsize);
}


/*******************************************************************************
函数名称：mem_malloc()
功    能：内存分配(内部调用)
说    明：
参    数: size，要分配的内存大小(字节)
返    回：0xFFFFFFFF,代表错误;其他,内存偏移地址
注    意：
*******************************************************************************/
uint32_t mem_malloc(uint32_t size)
{
    int32_t offset = 0;
    uint16_t nmemb  = 0;/* 需要的内存块数 */
    uint16_t cmemb  = 0;/* 连续空内存块数 */
    uint32_t i      = 0;

    if (!mallco_dev.memrdy)
    {
        mallco_dev.init();/* 未初始化,先执行初始化 */
    }

    if (size == 0)
    {
        return 0xFFFFFFFF;/* 不需要分配 */
    }

    nmemb = size / memblksize;    /* 获取需要分配的连续内存块数 */
    if (size % memblksize)
    {
        nmemb++;
    }

    /* 搜索整个内存控制区 */
    for (offset = memtblsize - 1; offset >= 0; offset--)
    {
        if (!mallco_dev.memmap[offset])
        {
            cmemb++;/* 连续空内存块数增加 */
        }
        else
        {
            cmemb = 0;                              /* 连续内存块清零 */
        }
        /* 找到了连续nmemb个空内存块 */
        if (cmemb == nmemb)
        {
            /* 标注内存块非空 */
            for (i = 0; i < nmemb; i++)
            {
                mallco_dev.memmap[offset + i] = nmemb;
            }

            return (offset * memblksize); /* 返回偏移地址 */
        }
    }

    return 0xFFFFFFFF;/* 未找到符合分配条件的内存块 */
}

/*******************************************************************************
函数名称：mem_free()
功    能：释放内存(内部调用)
说    明：
参    数: offset:内存地址偏移
返    回：0,释放成功;1,释放失败;
注    意：
*******************************************************************************/
uint8_t mem_free(uint32_t offset)
{
    int32_t i = 0;

    /* 未初始化,先执行初始化 */
    if (!mallco_dev.memrdy)
    {
        mallco_dev.init();
        return 1;/* 未初始化 */
    }

    /* 偏移在内存池内.  */
    if (offset < memsize)
    {
        int32_t index = offset / memblksize;         /* 偏移所在内存块号码 */
        int32_t nmemb = mallco_dev.memmap[index];    /* 内存块数量 */

        /* 内存块清零 */
        for (i = 0; i < nmemb; i++)
        {
            mallco_dev.memmap[index + i] = 0;
        }

        return 0;
    }
    else
    {
        return 2;/* 偏移超区了. */
    }
}

/*******************************************************************************
函数名称：myfree()
功    能：释放内存(外部调用)
说    明：
参    数: ptr:内存首地址
返    回：
注    意：
*******************************************************************************/
void myfree(void *ptr)
{
    uint32_t offset = 0;

    if (ptr == NULL)
    {
        /* 地址为0. */
        return;
    }

    offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase;
    /* 释放内存 */
    mem_free(offset);
}

/*******************************************************************************
函数名称：mymalloc()
功    能：分配内存(外部调用)
说    明：
参    数: size, 内存大小(字节)
返    回：分配到的内存首地址.
注    意：
*******************************************************************************/
void *mymalloc(uint32_t size)
{
    uint32_t offset = 0;
    offset = mem_malloc(size);

    if (offset == 0xFFFFFFFF)
    {
        return NULL;
    }
    else
    {
        return (void *)((uint32_t)mallco_dev.membase + offset);
    }
}

/*******************************************************************************
函数名称：myrealloc()
功    能：重新分配内存(外部调用)
说    明：
参    数: ptr:旧内存首地址, size:要分配的内存大小(字节)
返    回：新分配到的内存首地址.
注    意：
*******************************************************************************/
void *myrealloc(void *ptr, uint32_t size)
{
    uint32_t offset = 0;
    offset = mem_malloc(size);

    if (offset == 0xFFFFFFFF)
    {
        return NULL;
    }
    else
    {
        /* 拷贝旧内存内容到新内存 */
        mymemcpy((void *)((uint32_t)mallco_dev.membase + offset), ptr, size);
        /* 释放旧内存 */
        myfree(ptr);
        /* 返回新内存首地址 */
        return (void *)((uint32_t)mallco_dev.membase + offset);
    }
}

/************************ (C) COPYRIGHT Hopewellgroup ***********END OF FILE***/
