/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：LedBuzzer.h
* 描  述：点阵LED、数码管、独立LED和无源蜂鸣器的驱动模块头文件
* 版本号：v1.0.0
* 备  注：
*******************************************************************************
*/

#ifndef _LED_BUZZER_H
#define _LED_BUZZER_H

struct sLedBuff {  //LED显示缓冲区结构
    uint8 array[8];   //点阵缓冲区
    uint8 number[6];  //数码管缓冲区
    uint8 alone;      //独立LED缓冲区
};

#ifndef _LED_BUZZER_C
extern bit staBuzzer;
extern struct sLedBuff ledBuff;
#endif

void InitLed();
void FlowingLight();
void ShowLedNumber(uint8 index, uint8 num, uint8 point);
void ShowLedArray(uint8 *ptr);

#endif
