/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第10章 作业题3 数码管计时与流水灯同时运行的示例
* 版本号：v1.0.0
* 备  注：在数码管秒计时程序基础上添加流水灯功能，演示二者如何同时工作
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code LedChar[] = {  //数码管显示字符转换表
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[7] = {  //数码管+独立LED显示缓冲区，初值0xFF确保启动时都不亮
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char cnt200 = 0;  //200ms计数
unsigned int cnt1000 = 0;  //1000ms计数

void ShowCount();
void FlowingLight();

void main()
{
    EA = 1;       //使能总中断
    ENLED = 0;    //使能U3
    ADDR3 = 1;    //因为需要动态改变ADDR0-2的值，所以不需要再初始化了
    TMOD = 0x01;  //设置T0为模式1
    TH0  = 0xFC;  //为T0赋初值0xFC67，定时1ms
    TL0  = 0x67;
    ET0  = 1;     //使能T0中断
    TR0  = 1;     //启动T0
    
    while (1)
    {
        if (cnt200 >= 200)  //200ms刷新一次流水灯
        {
            cnt200 = 0;
            FlowingLight();
        }
        if (cnt1000 >= 1000)  //1s刷新一次计数值
        {
            cnt1000 = 0;
            ShowCount();
        }
    }
}
/* 流水灯实现函数 */
void FlowingLight()
{
    static unsigned char dir = 0;   //移位方向变量dir，用于控制移位的方向
    static unsigned char shift = 0x01;  //循环移位变量shift，并赋初值0x01

    LedBuff[6] = ~shift;      //循环移位变量取反，控制8个LED
    if (dir == 0)             //移位方向变量为0时，左移
    {
        shift = shift << 1;   //循环移位变量左移1位
        if (shift == 0x80)    //左移到最左端后，改变移位方向
        {
            dir = 1;
        }
    }
    else                      //移位方向变量不为0时，右移
    {
        shift = shift >> 1;   //循环移位变量右移1位
        if (shift == 0x01)    //右移到最右端后，改变移位方向
        {
            dir = 0;
        }
    }
}
/* 秒计数显示函数 */
void ShowCount()
{
    char i;  //循环变量
    unsigned char buf[6];   //中间转换缓冲区
    static unsigned long sec = 0;  //记录经过的秒数
    
    sec++;  //秒计数自加1
    buf[0] = sec%10;  //将sec按十进制位从低到高依次提取到buf数组中
    buf[1] = sec/10%10;
    buf[2] = sec/100%10;
    buf[3] = sec/1000%10;
    buf[4] = sec/10000%10;
    buf[5] = sec/100000%10;
    for (i=5; i>=1; i--)  //从最高为开始，遇到0不显示，遇到非0退出循环
    {
        if (buf[i] == 0)
            LedBuff[i] = 0xFF;
        else
            break;
    }
    for ( ; i>=0; i--)  //将剩余的有效数字位如实转换
    {
        LedBuff[i] = LedChar[buf[i]];
    }
}
/* 定时器0中断服务函数 */
void InterruptTimer0() interrupt 1
{
    static unsigned char i = 0;   //动态扫描的索引

    TH0 = 0xFC;  //重新加载初值
    TL0 = 0x67;
    cnt200++;
    cnt1000++;
    //以下代码完成数码管动态扫描刷新
    P0 = 0xFF;   //显示消隐
    switch (i)
    {
        case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=LedBuff[0]; break;
        case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=LedBuff[1]; break;
        case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=LedBuff[2]; break;
        case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=LedBuff[3]; break;
        case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=LedBuff[4]; break;
        case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=LedBuff[5]; break;
        case 6: ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=LedBuff[6]; break;
        default: break;
    }
}
