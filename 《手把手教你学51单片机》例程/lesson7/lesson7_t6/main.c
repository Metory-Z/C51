/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第7章 作业题6 
* 版本号：v1.0.0
* 备  注：独立LED、数码管、点阵LED同时全亮，其各自显示不同内容的方法会在后续例
*         程中反复用到，此时建议读者先自行考虑如何实现。
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

void main()
{
    EA = 1;       //使能总中断
    ENLED = 0;    //使能LED
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

    TH0 = 0xFC;  //重新加载初值
    TL0 = 0x67;
    //以下代码完成LED点阵动态扫描刷新
    P0 = 0xFF;   //显示消隐
    switch (i)
    {
        //点阵LED部分
        case 0: ADDR3 = 0; ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=0; break;
        case 1: ADDR3 = 0; ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=0; break;
        case 2: ADDR3 = 0; ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=0; break;
        case 3: ADDR3 = 0; ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=0; break;
        case 4: ADDR3 = 0; ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=0; break;
        case 5: ADDR3 = 0; ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=0; break;
        case 6: ADDR3 = 0; ADDR2=1; ADDR1=1; ADDR0=0; i++; P0=0; break;
        case 7: ADDR3 = 0; ADDR2=1; ADDR1=1; ADDR0=1; i++; P0=0; break;
        //数码管部分
        case 8:  ADDR3 = 1; ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=0; break;
        case 9:  ADDR3 = 1; ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=0; break;
        case 10: ADDR3 = 1; ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=0; break;
        case 11: ADDR3 = 1; ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=0; break;
        case 12: ADDR3 = 1; ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=0; break;
        case 13: ADDR3 = 1; ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=0; break;
        //独立LED部分
        case 14: ADDR3 = 1; ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=0; break;
        default: break;
    }
}
