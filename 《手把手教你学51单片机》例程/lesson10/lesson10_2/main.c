/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第10章 基于PWM方式控制LED的亮度
* 版本号：v1.0.0
* 备  注：详情见第10章10.2节
*******************************************************************************
*/

#include <reg52.h>

sbit PWMOUT = P0^0;
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char HighRH = 0;  //高电平重载值的高字节
unsigned char HighRL = 0;  //高电平重载值的低字节
unsigned char LowRH  = 0;  //低电平重载值的高字节
unsigned char LowRL  = 0;  //低电平重载值的低字节

void ConfigPWM(unsigned int fr, unsigned char dc);
void ClosePWM();

void main()
{
    unsigned int i;
    
    EA = 1;     //开总中断
    ENLED = 0;  //使能独立LED
    ADDR3 = 1;
    ADDR2 = 1;
    ADDR1 = 1;
    ADDR0 = 0;
    
    while (1)
    {
        ConfigPWM(100, 10);    //频率100Hz，占空比10%
        for (i=0; i<40000; i++);
        ClosePWM();
        ConfigPWM(100, 40);    //频率100Hz，占空比40%
        for (i=0; i<40000; i++);
        ClosePWM();
        ConfigPWM(100, 90);    //频率100Hz，占空比90%
        for (i=0; i<40000; i++);
        ClosePWM();            //关闭PWM，相当于占空比100%
        for (i=0; i<40000; i++);
    }
}
/* 配置并启动PWM，fr-频率，dc-占空比 */
void ConfigPWM(unsigned int fr, unsigned char dc)
{
    unsigned int  high, low;
    unsigned long tmp;
    
    tmp  = (11059200/12) / fr;  //计算一个周期所需的计数值
    high = (tmp*dc) / 100;      //计算高电平所需的计数值
    low  = tmp - high;          //计算低电平所需的计数值
    high = 65536 - high + 12;   //计算高电平的重载值并补偿中断延时
    low  = 65536 - low  + 12;   //计算低电平的重载值并补偿中断延时
    HighRH = (unsigned char)(high>>8); //高电平重载值拆分为高低字节
    HighRL = (unsigned char)high;
    LowRH  = (unsigned char)(low>>8);  //低电平重载值拆分为高低字节
    LowRL  = (unsigned char)low;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = HighRH;   //加载T0重载值
    TL0 = HighRL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
    PWMOUT = 1;     //输出高电平
}
/* 关闭PWM */
void ClosePWM()
{
    TR0 = 0;     //停止定时器
    ET0 = 0;     //禁止中断
    PWMOUT = 1;  //输出高电平
}
/* T0中断服务函数，产生PWM输出 */
void InterruptTimer0() interrupt 1
{
    if (PWMOUT == 1)  //当前输出为高电平时，装载低电平值并输出低电平
    {
        TH0 = LowRH;
        TL0 = LowRL;
        PWMOUT = 0;
    }
    else              //当前输出为低电平时，装载高电平值并输出高电平
    {
        TH0 = HighRH;
        TL0 = HighRL;
        PWMOUT = 1;
    }
}
