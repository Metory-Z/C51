/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第12章 1602液晶基本操作演示示例
* 版本号：v1.0.0
* 备  注：详情见第12章12.4节
*******************************************************************************
*/

#include <reg52.h>

#define LCD1602_DB  P0
sbit LCD1602_RS = P1^0;
sbit LCD1602_RW = P1^1;
sbit LCD1602_E  = P1^5;

void InitLcd1602();
void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

void main()
{
    unsigned char str[] = "Kingst Studio";

    InitLcd1602();
    LcdShowStr(2, 0, str);
    LcdShowStr(0, 1, "Welcome to KST51");
    while (1);
}
/* 等待液晶准备好 */
void LcdWaitReady()
{
    unsigned char sta;
    
    LCD1602_DB = 0xFF;
    LCD1602_RS = 0;
    LCD1602_RW = 1;
    do {
        LCD1602_E = 1;
        sta = LCD1602_DB; //读取状态字
        LCD1602_E = 0;
    } while (sta & 0x80); //bit7等于1表示液晶正忙，重复检测直到其等于0为止
}
/* 向LCD1602液晶写入一字节命令，cmd-待写入命令值 */
void LcdWriteCmd(unsigned char cmd)
{
    LcdWaitReady();
    LCD1602_RS = 0;
    LCD1602_RW = 0;
    LCD1602_DB = cmd;
    LCD1602_E  = 1;
    LCD1602_E  = 0;
}
/* 向LCD1602液晶写入一字节数据，dat-待写入数据值 */
void LcdWriteDat(unsigned char dat)
{
    LcdWaitReady();
    LCD1602_RS = 1;
    LCD1602_RW = 0;
    LCD1602_DB = dat;
    LCD1602_E  = 1;
    LCD1602_E  = 0;
}
/* 设置显示RAM起始地址，亦即光标位置，(x,y)-对应屏幕上的字符坐标 */
void LcdSetCursor(unsigned char x, unsigned char y)
{
    unsigned char addr;
    
    if (y == 0)  //由输入的屏幕坐标计算显示RAM的地址
        addr = 0x00 + x;  //第一行字符地址从0x00起始
    else
        addr = 0x40 + x;  //第二行字符地址从0x40起始
    LcdWriteCmd(addr | 0x80);  //设置RAM地址
}
/* 在液晶上显示字符串，(x,y)-对应屏幕上的起始坐标，str-字符串指针 */
void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str)
{
    LcdSetCursor(x, y);   //设置起始地址
    while (*str != '\0')  //连续写入字符串数据，直到检测到结束符
    {
        LcdWriteDat(*str++);  //先取str指向的数据，然后str自加1
    }
}
/* 初始化1602液晶 */
void InitLcd1602()
{
    LcdWriteCmd(0x38);  //16*2显示，5*7点阵，8位数据接口
    LcdWriteCmd(0x0C);  //显示器开，光标关闭
    LcdWriteCmd(0x06);  //文字不动，地址自动+1
    LcdWriteCmd(0x01);  //清屏
}
