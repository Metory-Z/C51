/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第11章 作业题6 UART串口控制蜂鸣器的开关
* 版本号：v1.0.0
* 备  注：只当接收到大写字符'B'后改变蜂鸣器的开/关状态
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit BUZZ  = P1^6;

unsigned char code LedChar[] = {  //数码管显示字符转换表
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[7] = {  //数码管+独立LED显示缓冲区
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节
unsigned char RxdByte = 0;  //串口接收到的字节
unsigned char flagBuzz = 0; //蜂鸣器控制标志

void ConfigTimer0(unsigned int ms);
void ConfigUART(unsigned int baud);

void main()
{
    EA = 1;       //使能总中断
    ENLED = 0;    //选择数码管和独立LED
    ADDR3 = 1;
    ConfigTimer0(1);   //配置T0定时1ms
    ConfigUART(9600);  //配置波特率为9600
    
    while (1)
    {   //将接收字节在数码管上以十六进制形式显示出来
        LedBuff[0] = LedChar[RxdByte & 0x0F];
        LedBuff[1] = LedChar[RxdByte >> 4];
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
/* 串口配置函数，baud-通信波特率 */
void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //配置串口为模式1
    TMOD &= 0x0F;  //清零T1的控制位
    TMOD |= 0x20;  //配置T1为模式2
    TH1 = 256 - (11059200/12/32)/baud;  //计算T1重载值
    TL1 = TH1;     //初值等于重载值
    ET1 = 0;       //禁止T1中断
    ES  = 1;       //使能串口中断
    TR1 = 1;       //启动T1
}
/* 按键扫描函数，需在定时中断中调用 */
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
/* T0中断服务函数，完成LED扫描 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    LedScan();   //LED扫描显示
    if (flagBuzz == 0)
        BUZZ = 1;      //蜂鸣器关闭
    else
        BUZZ = ~BUZZ;  //蜂鸣器鸣叫
}
/* UART中断服务函数 */
void InterruptUART() interrupt 4
{
    if (RI)  //接收到字节
    {
        RI = 0;  //手动清零接收中断标志位
        RxdByte = SBUF;  //接收到的数据保存到接收字节变量中
        SBUF = RxdByte;  //接收到的数据又直接发回，叫作-"echo"，
                         //用以提示用户输入的信息是否已正确接收
        if (RxdByte == 'B')  //接收到大写字母B时改变一次蜂鸣器控制标志
        {
            flagBuzz = !flagBuzz;
        }
    }
    if (TI)  //字节发送完毕
    {
        TI = 0;  //手动清零发送中断标志位
    }
}
