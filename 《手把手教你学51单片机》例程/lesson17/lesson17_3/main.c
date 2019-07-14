/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第17章 DA实现简易信号发生器
* 版本号：v1.0.0
* 备  注：详情见第17章17.7节
*         由按键控制DA输出可变化的波形
*******************************************************************************
*/

#include <reg52.h>

unsigned char code SinWave[] = {  //正弦波波表
    127, 152, 176, 198, 217, 233, 245, 252,
    255, 252, 245, 233, 217, 198, 176, 152,
    127, 102,  78,  56,  37,  21,   9,   2,
      0,   2,   9,  21,  37,  56,  78, 102,
};
unsigned char code TriWave[] = {  //三角波波表
      0,  16,  32,  48,  64,  80,  96, 112,
    128, 144, 160, 176, 192, 208, 224, 240,
    255, 240, 224, 208, 192, 176, 160, 144,
    128, 112,  96,  80,  64,  48,  32,  16,
};
unsigned char code SawWave[] = {  //锯齿波表
      0,   8,  16,  24,  32,  40,  48,  56,
     64,  72,  80,  88,  96, 104, 112, 120,
    128, 136, 144, 152, 160, 168, 176, 184,
    192, 200, 208, 216, 224, 232, 240, 248,
};
unsigned char code *pWave;  //波表指针
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节
unsigned char T1RH = 1;  //T1重载值的高字节
unsigned char T1RL = 1;  //T1重载值的低字节

void ConfigTimer0(unsigned int ms);
void SetWaveFreq(unsigned char freq);
extern void KeyScan();
extern void KeyDriver();
extern void I2CStart();
extern void I2CStop();
extern bit I2CWrite(unsigned char dat);

void main()
{    
    EA = 1;           //开总中断
    ConfigTimer0(1);  //配置T0定时1ms
    pWave = SinWave;  //默认正弦波
    SetWaveFreq(10);  //默认频率10Hz
    
    while (1)
    {
        KeyDriver();  //调用按键驱动
    }
}
/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
    static unsigned char i = 0;
    
    if (keycode == 0x26)  //向上键，切换波形
    {
        //在3种波形间循环切换
        if (i == 0)
        {
            i = 1;
            pWave = TriWave;
        }
        else if (i == 1)
        {
            i = 2;
            pWave = SawWave;
        }
        else
        {
            i = 0;
            pWave = SinWave;
        }
    }
}
/* 设置DAC输出值，val-设定值 */
void SetDACOut(unsigned char val)
{
    I2CStart();
    if (!I2CWrite(0x48<<1)) //寻址PCF8591，如未应答，则停止操作并返回
    {
        I2CStop();
        return;
    }
    I2CWrite(0x40);         //写入控制字节
    I2CWrite(val);          //写入DA值  
    I2CStop();
}
/* 设置输出波形的频率，freq-设定频率 */
void SetWaveFreq(unsigned char freq)
{
    unsigned long tmp;
    
    tmp = (11059200/12) / (freq*32); //定时器计数频率，是波形频率的32倍
    tmp = 65536 - tmp;               //计算定时器重载值
    tmp = tmp + 36;                  //修正中断响应延时造成的误差
    T1RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T1RL = (unsigned char)tmp;
    TMOD &= 0x0F;   //清零T1的控制位
    TMOD |= 0x10;   //配置T1为模式1
    TH1 = T1RH;     //加载T1重载值
    TL1 = T1RL;
    ET1 = 1;        //使能T1中断
    PT1 = 1;        //设置为高优先级
    TR1 = 1;        //启动T1
}
/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 28;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}
/* T0中断服务函数，执行按键扫描 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    KeyScan();   //按键扫描
}
/* T1中断服务函数，执行波形输出 */
void InterruptTimer1() interrupt 3
{
    static unsigned char i = 0;
    
    TH1 = T1RH;  //重新加载重载值
    TL1 = T1RL;
    //循环输出波表中的数据
    SetDACOut(pWave[i]);
    i++;
    if (i >= 32)
    {
        i = 0;
    }
}
