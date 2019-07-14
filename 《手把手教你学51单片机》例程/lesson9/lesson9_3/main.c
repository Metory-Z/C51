/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第9章 实用的步进电机驱动示例
* 版本号：v1.0.0
* 备  注：详情见第9章9.3.5节
*******************************************************************************
*/

#include <reg52.h>

unsigned long beats = 0;  //电机转动节拍总数

void StartMotor(unsigned long angle);

void main()
{
    EA = 1;       //使能总中断
    TMOD = 0x01;  //设置T0为模式1
    TH0  = 0xF8;  //为T0赋初值0xF8CD，定时2ms
    TL0  = 0xCD;
    ET0  = 1;     //使能T0中断
    TR0  = 1;     //启动T0
    
    StartMotor(360*2+180);  //控制电机转动2圈半
	while (1);
}
/* 步进电机启动函数，angle-需转过的角度 */
void StartMotor(unsigned long angle)
{
    //在计算前关闭中断，完成后再打开，以避免中断打断计算过程而造成错误
    EA = 0;
    beats = (angle * 4076) / 360; //实测为4076拍转动一圈
    EA = 1;
}
/* T0中断服务函数，用于驱动步进电机旋转 */
void InterruptTimer0() interrupt 1
{
    unsigned char tmp;  //临时变量
    static unsigned char index = 0;  //节拍输出索引
    unsigned char code BeatCode[8] = {  //步进电机节拍对应的IO控制代码
        0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
    };
    
    TH0 = 0xF8;  //重新加载初值
    TL0 = 0xCD;
    if (beats != 0)  //节拍数不为0则产生一个驱动节拍
    {
        tmp = P1;                    //用tmp把P1口当前值暂存
        tmp = tmp & 0xF0;            //用&操作清零低4位
        tmp = tmp | BeatCode[index]; //用|操作把节拍代码写到低4位
        P1  = tmp;                   //把低4位的节拍代码和高4位的原值送回P1
        index++;                     //节拍输出索引递增
        index = index & 0x07;        //用&操作实现到8归零
        beats--;                     //总节拍数-1
    }
    else  //节拍数为0则关闭电机所有的相
    {
        P1 = P1 | 0x0F;
    }
}
