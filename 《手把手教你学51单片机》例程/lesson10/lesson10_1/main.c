/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第10章 基于数码管显示的数字秒表
* 版本号：v1.0.0
* 备  注：详情见第10章10.1节
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit KEY1 = P2^4;
sbit KEY2 = P2^5;
sbit KEY3 = P2^6;
sbit KEY4 = P2^7;

unsigned char code LedChar[] = {  //数码管显示字符转换表
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = {  //数码管显示缓冲区
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char KeySta[4] = {  //按键当前状态
    1, 1, 1, 1
};

bit StopwatchRunning = 0;  //秒表运行标志
bit StopwatchRefresh = 1;  //秒表计数刷新标志
unsigned char DecimalPart = 0;  //秒表的小数部分
unsigned int  IntegerPart = 0;  //秒表的整数部分
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

void ConfigTimer0(unsigned int ms);
void StopwatchDisplay();
void KeyDriver();

void main()
{
    EA = 1;      //开总中断
    ENLED = 0;   //使能选择数码管
    ADDR3 = 1;
    P2 = 0xFE;   //P2.0置0，选择第4行按键作为独立按键
	ConfigTimer0(2);  //配置T0定时2ms
    
    while (1)
    {
        if (StopwatchRefresh)  //需要刷新秒表示数时调用显示函数
        {
            StopwatchRefresh = 0;
            StopwatchDisplay();
        }
        KeyDriver();  //调用按键驱动函数
    }
}
/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 18;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}
/* 秒表计数显示函数 */
void StopwatchDisplay()
{
	signed char i;
	unsigned char buf[4];  //数据转换的缓冲区
    
    //小数部分转换到低2位
    LedBuff[0] = LedChar[DecimalPart%10];
    LedBuff[1] = LedChar[DecimalPart/10];
    //整数部分转换到高4位
    buf[0] = IntegerPart%10;
    buf[1] = (IntegerPart/10)%10;
    buf[2] = (IntegerPart/100)%10;
    buf[3] = (IntegerPart/1000)%10;
    for (i=3; i>=1; i--)  //整数部分高位的0转换为空字符
    {
        if (buf[i] == 0)
            LedBuff[i+2] = 0xFF;
        else
            break;
    }
    for ( ; i>=0; i--)  //有效数字位转换为显示字符
    {
        LedBuff[i+2] = LedChar[buf[i]];
    }
    LedBuff[2] &= 0x7F;  //点亮小数点
}
/* 秒表启停函数 */
void StopwatchAction()
{
    if (StopwatchRunning)    //已启动则停止
        StopwatchRunning = 0;
    else                     //未启动则启动
        StopwatchRunning = 1;
}
/* 秒表复位函数 */
void StopwatchReset()
{
    StopwatchRunning = 0;  //停止秒表
    DecimalPart = 0;       //清零计数值
    IntegerPart = 0;
    StopwatchRefresh = 1;  //置刷新标志
}
/* 按键驱动函数，检测按键动作，调度相应动作函数，需在主循环中调用 */
void KeyDriver()
{
    unsigned char i;
    static unsigned char backup[4] = {1,1,1,1};

    for (i=0; i<4; i++)  //循环检测4个按键
    {
        if (backup[i] != KeySta[i])  //检测按键动作
        {
            if (backup[i] != 0)      //按键按下时执行动作
            {
                if (i == 1)          //Esc键复位秒表
                    StopwatchReset();
                else if (i == 2)     //回车键启停秒表
                    StopwatchAction();
            }
            backup[i] = KeySta[i];   //刷新前一次的备份值
        }
    }
}
/* 按键扫描函数，需在定时中断中调用 */
void KeyScan()
{
    unsigned char i;
    static unsigned char keybuf[4] = {  //按键扫描缓冲区
        0xFF, 0xFF, 0xFF, 0xFF
    };
    
    //按键值移入缓冲区
    keybuf[0] = (keybuf[0] << 1) | KEY1;
    keybuf[1] = (keybuf[1] << 1) | KEY2;
    keybuf[2] = (keybuf[2] << 1) | KEY3;
    keybuf[3] = (keybuf[3] << 1) | KEY4;
    //消抖后更新按键状态
    for (i=0; i<4; i++)
    {
        if (keybuf[i] == 0x00)
        {   //连续8次扫描值为0，即16ms内都是按下状态时，可认为按键已稳定的按下
            KeySta[i] = 0;
        }
        else if (keybuf[i] == 0xFF)
        {   //连续8次扫描值为1，即16ms内都是弹起状态时，可认为按键已稳定的弹起
            KeySta[i] = 1;
        }
    }
}
/* 数码管动态扫描刷新函数，需在定时中断中调用 */
void LedScan()
{
    static unsigned char i = 0;  //动态扫描索引
    
    P0 = 0xFF;             //关闭所有段选位，显示消隐
    P1 = (P1 & 0xF8) | i;  //位选索引值赋值到P1口低3位
    P0 = LedBuff[i];       //缓冲区中索引位置的数据送到P0口
    if (i < 5)             //索引递增循环，遍历整个缓冲区
        i++;
    else
        i = 0;
}
/* 秒表计数函数，每隔10ms调用一次进行秒表计数累加 */
void StopwatchCount()
{
    if (StopwatchRunning)  //当处于运行状态时递增计数值
    {
        DecimalPart++;           //小数部分+1
        if (DecimalPart >= 100)  //小数部分计到100时进位到整数部分
        {
            DecimalPart = 0;
            IntegerPart++;       //整数部分+1
            if (IntegerPart >= 10000)  //整数部分计到10000时归零
            {
                IntegerPart = 0;
            }
        }
        StopwatchRefresh = 1;    //设置秒表计数刷新标志
    }
}
/* T0中断服务函数，完成数码管、按键扫描与秒表计数 */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr10ms = 0;

    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    LedScan();   //数码管扫描显示
    KeyScan();   //按键扫描
    //定时10ms进行一次秒表计数
    tmr10ms++;
    if (tmr10ms >= 5)
    {
        tmr10ms = 0;
        StopwatchCount();  //调用秒表计数函数
    }
}
