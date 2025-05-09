#include "myString.h"

/*
    字符串比较
    如果dest > source,则返回值大于0，
    如果dest = source,则返回值等于0，
    如果dest < source ,则返回值小于0
*/
uint8_t my_strcmp(uint8_t *dest,uint8_t *source)
{
	while (*dest && *source && (*dest == *source))
    {
        dest ++;
        source ++;
    }

    return *dest - *source;
}

/*
    在源字符串中查找字符
    返回 字符在源字符串中的下标号
*/
uint8_t findStr(char * func,char str)
{
    char *pFunc=func;
    uint8_t find_num = 0;

    while(*pFunc != '\0')
    {
        if(*pFunc == str)//找到字符
        {
            break;//找到空格跳出
        }
 
        find_num++;
        pFunc++;
    }

    return find_num;//返回数组下标
}

//查找字符出现的次数 
uint8_t findstrnum(char * func,char str)
{
    char *pFunc=func;
    uint8_t find_num = 0;
    while(*pFunc != '\0')
    {
        if(*pFunc == str)//
        {
            find_num++;
        }
        pFunc++;
    }

    return find_num;
}

/*
    将字符串转换成数字
*/
int strToint(char *s,uint8_t num)
{
    int n =0;
    for(uint8_t i=0;i<num;i++)
    {
        if((s[i] >= '0') && (s[i] <= '9'))
        {
            n = 10 * n + (s[i] - '0');
        }
    }
    
    return n; 
}

//查找字符串中的函数名称
void findfuncname(char * str,char *str1)
{
    //找到空格
    uint8_t kong_ge = findStr(str,' ');
    //找到函数的名称
    uint8_t zuo_kuoHao = findStr(str,'(');

    if((kong_ge > 0) && (kong_ge < zuo_kuoHao))//有返回类型
    {
        uint8_t strlen = zuo_kuoHao-kong_ge-1;//得到当前字符串长度
        memcpy(str1,str+kong_ge+1,strlen);
        str1[kong_ge+1+strlen+1]='\0';
    }
    else//无返回类型
    {
        uint8_t strlen = zuo_kuoHao;//得到当前字符串长度
        memcpy(str1,str,strlen);
        str1[strlen+1]='\0';
    }
}


////获取下一个字符（当中间有很多空格的时候，此函数直接忽略空格，找到空格之后的第一个字符）
////str:字符串指针	
////返回值:下一个字符
//uint8_t usmart_search_nextc(uint8_t* str)
//{		   	 	
//	str++;
//	while(*str==' '&&str!='\0')str++;
//	return *str;
//}


//找字符 返回下标 
//func:字符串
//num:找第几个
//str:查找目标
uint8_t findStrS(char * func,uint8_t num,char str)
{
    char *pFunc=func;
    uint8_t find_num = 0;
    uint8_t find_numN= 0;

    while(*pFunc != '\0')
    {
        if(*pFunc == str)
        {
            find_numN++;
            if(find_numN == num)
            {
                break;
            }
        }
 
        find_num++;
        pFunc++;
    }

    return find_num;
}


