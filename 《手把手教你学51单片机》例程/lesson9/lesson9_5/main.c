/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第9章 蜂鸣器驱动的基础示例
* 版本号：v1.0.0
* 备  注：详情见第9章9.4节
*******************************************************************************
*/

#include <reg52.h>

sbit BUZZ = P1^6;  //蜂鸣器控制引脚
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

void OpenBuzz(unsigned int frequ);
void StopBuzz();

void main()
{
    unsigned int i;
    
    TMOD = 0x01;  //配置T0工作在模式1，但先不启动
    EA = 1;       //使能全局中断
    
    while (1)
    {
        OpenBuzz(4000);          //以4KHz的频率启动蜂鸣器
        for (i=0; i<40000; i++);
        StopBuzz();              //停止蜂鸣器
        for (i=0; i<40000; i++);
        OpenBuzz(1000);          //以1KHz的频率启动蜂鸣器
        for (i=0; i<40000; i++);
        StopBuzz();              //停止蜂鸣器
        for (i=0; i<40000; i++);
    }
}
/* 蜂鸣器启动函数，frequ-工作频率 */
void OpenBuzz(unsigned int frequ)
{
    unsigned int reload;    //计算所需的定时器重载值
    
    reload = 65536 - (11059200/12)/(frequ*2);  //由给定频率计算定时器重载值
    T0RH = (unsigned char)(reload >> 8);  //16位重载值分解为高低两个字节
    T0RL = (unsigned char)reload;
    TH0  = 0xFF;  //设定一个接近溢出的初值，以使定时器马上投入工作
    TL0  = 0xFE;
    ET0  = 1;     //使能T0中断
    TR0  = 1;     //启动T0
}
/* 蜂鸣器停止函数 */
void StopBuzz()
{
    ET0 = 0;   //禁用T0中断
    TR0 = 0;   //停止T0
}
/* T0中断服务函数，用于控制蜂鸣器发声 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;   //重新加载重载值
    TL0 = T0RL;
    BUZZ = ~BUZZ; //反转蜂鸣器控制电平
}
