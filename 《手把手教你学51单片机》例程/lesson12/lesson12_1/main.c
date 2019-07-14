/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第12章 指针作为函数参数的示例
* 版本号：v1.0.0
* 备  注：详情见第12章12.1节
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

void ShiftLeft(unsigned char *p);

void main()
{
    unsigned int  i;
    unsigned char buf = 0x01;
    
    ENLED = 0;   //使能选择独立LED
    ADDR3 = 1;
    ADDR2 = 1;
    ADDR1 = 1;
    ADDR0 = 0;
    
    while (1)
    {
        P0 = ~buf;               //缓冲值取反送到P0口
        for (i=0; i<20000; i++); //延时
        ShiftLeft(&buf);         //缓冲值左移一位
        if (buf == 0)            //如移位后为0则重赋初值
        {
            buf = 0x01;
        }
    }
}
/* 将指针变量p指向的字节左移一位 */
void ShiftLeft(unsigned char *p)
{
    *p = *p << 1;  //利用指针变量可以向函数外输出运算结果
}
