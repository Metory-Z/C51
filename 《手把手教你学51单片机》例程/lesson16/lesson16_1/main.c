/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第16章 红外遥控器接收示例
* 版本号：v1.0.0
* 备  注：详情见第16章16.3节
*         接收并解析NEC协议的红外编码，并将用户码和键码显示到数码管上
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code LedChar[] = {  //数码管显示字符转换表
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = {  //数码管显示缓冲区
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

extern bit irflag;
extern unsigned char ircode[4];
extern void InitInfrared();
void ConfigTimer0(unsigned int ms);

void main()
{
    EA = 1;      //开总中断
    ENLED = 0;   //使能选择数码管
    ADDR3 = 1;
    InitInfrared();   //初始化红外功能
    ConfigTimer0(1);  //配置T0定时1ms
    //PT0 = 1;        //配置T0中断为高优先级，启用本行可消除接收时的闪烁
    
    while (1)
    {
        if (irflag)  //接收到红外数据时刷新显示
        {
            irflag = 0;
            LedBuff[5] = LedChar[ircode[0] >> 4];  //用户码显示
            LedBuff[4] = LedChar[ircode[0]&0x0F];
            LedBuff[1] = LedChar[ircode[2] >> 4];  //键码显示
            LedBuff[0] = LedChar[ircode[2]&0x0F];
        }
    }
}
/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 13;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}
/* LED动态扫描刷新函数，需在定时中断中调用 */
void LedScan()
{
    static unsigned char i = 0;  //动态扫描索引
    
    P0 = 0xFF;                  //关闭所有段选位，显示消隐
    P1 = (P1 & 0xF8) | i;       //位选索引值赋值到P1口低3位
    P0 = LedBuff[i];            //缓冲区中索引位置的数据送到P0口
    if (i < sizeof(LedBuff)-1)  //索引递增循环，遍历整个缓冲区
        i++;
    else
        i = 0;
}
/* T0中断服务函数，执行数码管扫描显示 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    LedScan();   //数码管扫描显示
}
