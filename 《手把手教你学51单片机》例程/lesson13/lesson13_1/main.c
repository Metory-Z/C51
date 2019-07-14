/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第13章 1602液晶整屏移动示例
* 版本号：v1.0.0
* 备  注：详情见第13章13.2节
*         1602液晶显示两行字符串，并实现整屏的重复左移
*******************************************************************************
*/

#include <reg52.h>

#define LCD1602_DB  P0
sbit LCD1602_RS = P1^0;
sbit LCD1602_RW = P1^1;
sbit LCD1602_E  = P1^5;

bit flag500ms = 0;   //500ms定时标志
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节
//待显示的第一行字符串
unsigned char code str1[] = "Kingst Studio";
//待显示的第二行字符串，需保持与第一行字符串等长，较短的行可用空格补齐
unsigned char code str2[] = "Let's move...";

void ConfigTimer0(unsigned int ms);
void InitLcd1602();
void LcdShowStr(unsigned char x, unsigned char y,
                unsigned char *str, unsigned char len);

void main()
{
    unsigned char i;
    unsigned char index = 0;  //移动索引
    unsigned char pdata bufMove1[16+sizeof(str1)+16]; //移动显示缓冲区1
    unsigned char pdata bufMove2[16+sizeof(str2)+16]; //移动显示缓冲区2

    EA = 1;            //开总中断
    ConfigTimer0(10);  //配置T0定时10ms
    InitLcd1602();     //初始化液晶
    //缓冲区开头一段填充为空格
    for (i=0; i<16; i++)
    {
        bufMove1[i] = ' ';
        bufMove2[i] = ' ';
    }
    //待显示字符串拷贝到缓冲区中间位置
    for (i=0; i<(sizeof(str1)-1); i++)
    {
        bufMove1[16+i] = str1[i];
        bufMove2[16+i] = str2[i];
    }
    //缓冲区结尾一段也填充为空格
    for (i=(16+sizeof(str1)-1); i<sizeof(bufMove1); i++)
    {
        bufMove1[i] = ' ';
        bufMove2[i] = ' ';
    }
    
    while (1)
    {
        if (flag500ms)  //每500ms移动一次屏幕
        {
            flag500ms = 0;
            //从缓冲区抽出需显示的一段字符显示到液晶上
            LcdShowStr(0, 0, bufMove1+index, 16);
            LcdShowStr(0, 1, bufMove2+index, 16);
            //移动索引递增，实现左移
            index++;
            if (index >= (16+sizeof(str1)-1))
            {   //起始位置达到字符串尾部后即返回从头开始
                index = 0;
            }
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
    tmp = tmp + 12;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
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
/* 在液晶上显示字符串，(x,y)-对应屏幕上的起始坐标，
   str-字符串指针，len-需显示的字符长度 */
void LcdShowStr(unsigned char x, unsigned char y, 
                unsigned char *str, unsigned char len)
{
    LcdSetCursor(x, y);   //设置起始地址
    while (len--)         //连续写入len个字符数据
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
/* T0中断服务函数，定时500ms */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr500ms = 0;
    
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    tmr500ms++;
    if (tmr500ms >= 50)
    {
        tmr500ms = 0;
        flag500ms = 1;
    }
}
