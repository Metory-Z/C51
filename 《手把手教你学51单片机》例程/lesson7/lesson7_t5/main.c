/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第7章 作业题5 
* 版本号：v1.0.0
* 备  注：基于LED点阵的9～0倒计数
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code image[11][8] = {
    {0xC3, 0x81, 0x99, 0x99, 0x99, 0x99, 0x81, 0xC3},  //数字0
    {0xEF, 0xE7, 0xE3, 0xE7, 0xE7, 0xE7, 0xE7, 0xC3},  //数字1
    {0xC3, 0x81, 0x9D, 0x87, 0xC3, 0xF9, 0xC1, 0x81},  //数字2
    {0xC3, 0x81, 0x9D, 0xC7, 0xC7, 0x9D, 0x81, 0xC3},  //数字3
    {0xCF, 0xC7, 0xC3, 0xC9, 0xC9, 0x81, 0xCF, 0xCF},  //数字4
    {0x81, 0xC1, 0xF9, 0xC3, 0x87, 0x9D, 0x81, 0xC3},  //数字5
    {0xC3, 0x81, 0xF9, 0xC1, 0x81, 0x99, 0x81, 0xC3},  //数字6
    {0x81, 0x81, 0x9F, 0xCF, 0xCF, 0xE7, 0xE7, 0xE7},  //数字7
    {0xC3, 0x81, 0x99, 0xC3, 0xC3, 0x99, 0x81, 0xC3},  //数字8
    {0xC3, 0x81, 0x99, 0x81, 0x83, 0x9F, 0x83, 0xC1},  //数字9
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //全亮
};

void main()
{
    EA = 1;       //使能总中断
    ENLED = 0;    //使能U4，选择LED点阵
    ADDR3 = 0;
    TMOD = 0x01;  //设置T0为模式1
    TH0  = 0xFC;  //为T0赋初值0xFC67，定时1ms
    TL0  = 0x67;
    ET0  = 1;     //使能T0中断
    TR0  = 1;     //启动T0
    while (1);
}
/* 定时器0中断服务函数 */
void InterruptTimer0() interrupt 1
{
    static unsigned char i = 0;  //动态扫描的索引
    static unsigned int  tmr = 0;  //1s软件定时器
    static unsigned char index = 9;  //图片刷新索引

    TH0 = 0xFC;  //重新加载初值
    TL0 = 0x67;
    //以下代码完成LED点阵动态扫描刷新
    P0 = 0xFF;   //显示消隐
    switch (i)
    {
        case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=image[index][0]; break;
        case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=image[index][1]; break;
        case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=image[index][2]; break;
        case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=image[index][3]; break;
        case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=image[index][4]; break;
        case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=image[index][5]; break;
        case 6: ADDR2=1; ADDR1=1; ADDR0=0; i++; P0=image[index][6]; break;
        case 7: ADDR2=1; ADDR1=1; ADDR0=1; i=0; P0=image[index][7]; break;
        default: break;
    }
    //以下代码完成每秒改变一帧图像
    tmr++;
    if (tmr >= 1000)  //达到1000ms时改变一次图片索引
    {
        tmr = 0;
        if (index == 0)  //图片索引10~0循环
            index = 10;
        else
            index--;
    }
}
