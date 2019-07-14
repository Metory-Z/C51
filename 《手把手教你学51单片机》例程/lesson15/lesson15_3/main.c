/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第15章 用DS1302实现简易电子钟
* 版本号：v1.0.0
* 备  注：详情见第15章15.5节
*         用液晶显示日期时间，并可通过按键校时
*******************************************************************************
*/

#include <reg52.h>

struct sTime {  //日期时间结构体定义
    unsigned int  year;
    unsigned char mon;
    unsigned char day;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    unsigned char week;
};

bit flag200ms = 1;  //200ms定时标志
struct sTime bufTime;  //日期时间缓冲区
unsigned char setIndex = 0;  //时间设置索引
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

void ConfigTimer0(unsigned int ms);
void RefreshTimeShow();
extern void InitDS1302();
extern void GetRealTime(struct sTime *time);
extern void SetRealTime(struct sTime *time);
extern void KeyScan();
extern void KeyDriver();
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdSetCursor(unsigned char x, unsigned char y);
extern void LcdOpenCursor();
extern void LcdCloseCursor();

void main()
{
    unsigned char psec=0xAA;  //秒备份，初值AA确保首次读取时间后会刷新显示

    EA = 1;           //开总中断
    ConfigTimer0(1);  //T0定时1ms
    InitDS1302();     //初始化实时时钟
    InitLcd1602();    //初始化液晶
    
    //初始化屏幕上固定不变的内容
    LcdShowStr(3, 0, "20  -  -  ");
    LcdShowStr(4, 1, "  :  :  ");
    
    while (1)
    {
        KeyDriver();  //调用按键驱动
        if (flag200ms && (setIndex == 0))
        {                             //每隔200ms且未处于设置状态时，
            flag200ms = 0;
            GetRealTime(&bufTime);    //获取当前时间
            if (psec != bufTime.sec)  //检测到时间有变化时刷新显示
            {
                RefreshTimeShow();
                psec = bufTime.sec;   //用当前值更新上次秒数
            }
        }
    }
}
/* 将一个BCD码字节显示到屏幕上，(x,y)-屏幕起始坐标，bcd-待显示BCD码 */
void ShowBcdByte(unsigned char x, unsigned char y, unsigned char bcd)
{
    unsigned char str[4];
    
    str[0] = (bcd >> 4) + '0';
    str[1] = (bcd&0x0F) + '0';
    str[2] = '\0';
    LcdShowStr(x, y, str);
}
/* 刷新日期时间的显示 */
void RefreshTimeShow()
{
    ShowBcdByte(5,  0, bufTime.year);
    ShowBcdByte(8,  0, bufTime.mon);
    ShowBcdByte(11, 0, bufTime.day);
    ShowBcdByte(4,  1, bufTime.hour);
    ShowBcdByte(7,  1, bufTime.min);
    ShowBcdByte(10, 1, bufTime.sec);
}
/* 刷新当前设置位的光标指示 */
void RefreshSetShow()
{
    switch (setIndex)
    {
        case 1:  LcdSetCursor(5,  0); break;
        case 2:  LcdSetCursor(6,  0); break;
        case 3:  LcdSetCursor(8,  0); break;
        case 4:  LcdSetCursor(9,  0); break;
        case 5:  LcdSetCursor(11, 0); break;
        case 6:  LcdSetCursor(12, 0); break;
        case 7:  LcdSetCursor(4,  1); break;
        case 8:  LcdSetCursor(5,  1); break;
        case 9:  LcdSetCursor(7,  1); break;
        case 10: LcdSetCursor(8,  1); break;
        case 11: LcdSetCursor(10, 1); break;
        case 12: LcdSetCursor(11, 1); break;
        default:  break;
    }
}
/* 递增一个BCD码的高位 */
unsigned char IncBcdHigh(unsigned char bcd)
{
    if ((bcd&0xF0) < 0x90)
        bcd += 0x10;
    else
        bcd &= 0x0F;
    
    return bcd;
}
/* 递增一个BCD码的低位 */
unsigned char IncBcdLow(unsigned char bcd)
{
    if ((bcd&0x0F) < 0x09)
        bcd += 0x01;
    else
        bcd &= 0xF0;
    
    return bcd;
}
/* 递减一个BCD码的高位 */
unsigned char DecBcdHigh(unsigned char bcd)
{
    if ((bcd&0xF0) > 0x00)
        bcd -= 0x10;
    else
        bcd |= 0x90;
    
    return bcd;
}
/* 递减一个BCD码的低位 */
unsigned char DecBcdLow(unsigned char bcd)
{
    if ((bcd&0x0F) > 0x00)
        bcd -= 0x01;
    else
        bcd |= 0x09;
    
    return bcd;
}
/* 递增时间当前设置位的值 */
void IncSetTime()
{
    switch (setIndex)
    {
        case 1:  bufTime.year = IncBcdHigh(bufTime.year); break;
        case 2:  bufTime.year = IncBcdLow(bufTime.year);  break;
        case 3:  bufTime.mon  = IncBcdHigh(bufTime.mon);  break;
        case 4:  bufTime.mon  = IncBcdLow(bufTime.mon);   break;
        case 5:  bufTime.day  = IncBcdHigh(bufTime.day);  break;
        case 6:  bufTime.day  = IncBcdLow(bufTime.day);   break;
        case 7:  bufTime.hour = IncBcdHigh(bufTime.hour); break;
        case 8:  bufTime.hour = IncBcdLow(bufTime.hour);  break;
        case 9:  bufTime.min  = IncBcdHigh(bufTime.min);  break;
        case 10: bufTime.min  = IncBcdLow(bufTime.min);   break;
        case 11: bufTime.sec  = IncBcdHigh(bufTime.sec);  break;
        case 12: bufTime.sec  = IncBcdLow(bufTime.sec);   break;
        default:  break;
    }
    RefreshTimeShow();
    RefreshSetShow();
}
/* 递减时间当前设置位的值 */
void DecSetTime()
{
    switch (setIndex)
    {
        case 1:  bufTime.year = DecBcdHigh(bufTime.year); break;
        case 2:  bufTime.year = DecBcdLow(bufTime.year);  break;
        case 3:  bufTime.mon  = DecBcdHigh(bufTime.mon);  break;
        case 4:  bufTime.mon  = DecBcdLow(bufTime.mon);   break;
        case 5:  bufTime.day  = DecBcdHigh(bufTime.day);  break;
        case 6:  bufTime.day  = DecBcdLow(bufTime.day);   break;
        case 7:  bufTime.hour = DecBcdHigh(bufTime.hour); break;
        case 8:  bufTime.hour = DecBcdLow(bufTime.hour);  break;
        case 9:  bufTime.min  = DecBcdHigh(bufTime.min);  break;
        case 10: bufTime.min  = DecBcdLow(bufTime.min);   break;
        case 11: bufTime.sec  = DecBcdHigh(bufTime.sec);  break;
        case 12: bufTime.sec  = DecBcdLow(bufTime.sec);   break;
        default:  break;
    }
    RefreshTimeShow();
    RefreshSetShow();
}
/* 右移时间设置位 */
void RightShiftTimeSet()
{
    if (setIndex != 0)
    {
        if (setIndex < 12)
            setIndex++;
        else
            setIndex = 1;
        RefreshSetShow();
    }
}
/* 左移时间设置位 */
void LeftShiftTimeSet()
{
    if (setIndex != 0)
    {
        if (setIndex > 1)
            setIndex--;
        else
            setIndex = 12;
        RefreshSetShow();
    }
}
/* 进入时间设置状态 */
void EnterTimeSet()
{
    setIndex = 2;       //把设置索引设置为2，即可进入设置状态
    LeftShiftTimeSet(); //再利用现成的左移操作移到位置1并完成显示刷新
    LcdOpenCursor();    //打开光标闪烁效果
}
/* 退出时间设置状态，save-是否保存当前设置的时间值 */
void ExitTimeSet(bit save)
{
    setIndex = 0;     //把设置索引设置为0，即可退出设置状态
    if (save)         //需保存时即把当前设置时间写入DS1302
    {
        SetRealTime(&bufTime);
    }
    LcdCloseCursor(); //关闭光标显示
}
/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
    if  ((keycode>='0') && (keycode<='9'))  //本例中不响应字符键
    {
    }
    else if (keycode == 0x26)  //向上键，递增当前设置位的值
    {
        IncSetTime();
    }
    else if (keycode == 0x28)  //向下键，递减当前设置位的值
    {
        DecSetTime();
    }
    else if (keycode == 0x25)  //向左键，向左切换设置位
    {
        LeftShiftTimeSet();
    }
    else if (keycode == 0x27)  //向右键，向右切换设置位
    {
        RightShiftTimeSet();
    }
    else if (keycode == 0x0D)  //回车键，进入设置模式/启用当前设置值
    {
        if (setIndex == 0)  //不处于设置状态时，进入设置状态
        {
            EnterTimeSet();
        }
        else                //已处于设置状态时，保存时间并退出设置状态
        {
            ExitTimeSet(1);
        }
    }
    else if (keycode == 0x1B)  //Esc键，取消当前设置
    {
        ExitTimeSet(0);
    }
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
/* T0中断服务函数，执行按键扫描和200ms定时 */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr200ms = 0;
    
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    KeyScan();   //按键扫描
    tmr200ms++;
    if (tmr200ms >= 200)  //定时200ms
    {
        tmr200ms = 0;
        flag200ms = 1;
    }
}
