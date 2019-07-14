/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第10章 交通信号灯示例
* 版本号：v1.0.0
* 备  注：详情见第10章10.3节
*******************************************************************************
*/

#include <reg52.h>

sbit  ADDR3 = P1^3;
sbit  ENLED = P1^4;

unsigned char code LedChar[] = {  //数码管显示字符转换表
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[7] = {  //数码管+独立LED显示缓冲区
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
bit flag1s = 1;          //1秒定时标志
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

void ConfigTimer0(unsigned int ms);
void TrafficLight();

void main()
{
    EA = 1;      //开总中断
    ENLED = 0;   //使能数码管和LED
    ADDR3 = 1;
	ConfigTimer0(1);  //配置T0定时1ms
    
    while (1)
    {
        if (flag1s)  //每秒执行一次交通灯刷新
        {
            flag1s = 0;
            TrafficLight();
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
/* 交通灯显示刷新函数 */
void TrafficLight()
{
    static unsigned char color = 2;  //颜色索引：0-绿色/1-黄色/2-红色
    static unsigned char timer = 0;  //倒计时定时器
    
    if (timer == 0) //倒计时到0时，切换交通灯
    {
        switch (color)  //LED8/9代表绿灯，LED5/6代表黄灯，LED2/3代表红灯
        {
            case 0:     //切换到黄色，亮3秒
                color = 1;
                timer = 2;
                LedBuff[6] = 0xE7;
                break;
            case 1:     //切换到红色，亮30秒
                color = 2;
                timer = 29;
                LedBuff[6] = 0xFC;
                break;
            case 2:     //切换到绿色，亮40秒
                color = 0;
                timer = 39;
                LedBuff[6] = 0x3F;
                break;
            default:
                break;
        }
    }
    else  //倒计时未到0时，递减其计数值
    {
        timer--;
    }
    LedBuff[0] = LedChar[timer%10];  //倒计时数值个位显示
    LedBuff[1] = LedChar[timer/10];  //倒计时数值十位显示
}
/* LED动态扫描刷新函数，需在定时中断中调用 */
void LedScan()
{
    static unsigned char i = 0;  //动态扫描索引
    
    P0 = 0xFF;             //关闭所有段选位，显示消隐
    P1 = (P1 & 0xF8) | i;  //位选索引值赋值到P1口低3位
    P0 = LedBuff[i];       //缓冲区中索引位置的数据送到P0口
    if (i < 6)             //索引递增循环，遍历整个缓冲区
        i++;
    else
        i = 0;
}
/* T0中断服务函数，完成LED扫描和秒定时 */
void InterruptTimer0() interrupt 1
{
    static unsigned int tmr1s = 0;  //1秒定时器

    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    LedScan();   //LED扫描显示
    tmr1s++;     //1秒定时的处理
    if (tmr1s >= 1000)
    {
        tmr1s = 0;
        flag1s = 1;  //设置秒定时标志
    }
}
