/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第8章 基于矩阵按键和数码管实现的简易加法计算器
* 版本号：v1.0.0
* 备  注：详情见第8章8.5节
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit KEY_IN_1  = P2^4;
sbit KEY_IN_2  = P2^5;
sbit KEY_IN_3  = P2^6;
sbit KEY_IN_4  = P2^7;
sbit KEY_OUT_1 = P2^3;
sbit KEY_OUT_2 = P2^2;
sbit KEY_OUT_3 = P2^1;
sbit KEY_OUT_4 = P2^0;

unsigned char code LedChar[] = {  //数码管显示字符转换表
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = {  //数码管显示缓冲区
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char code KeyCodeMap[4][4] = { //矩阵按键编号到标准键盘键码的映射表
    { 0x31, 0x32, 0x33, 0x26 }, //数字键1、数字键2、数字键3、向上键
    { 0x34, 0x35, 0x36, 0x25 }, //数字键4、数字键5、数字键6、向左键
    { 0x37, 0x38, 0x39, 0x28 }, //数字键7、数字键8、数字键9、向下键
    { 0x30, 0x1B, 0x0D, 0x27 }  //数字键0、ESC键、  回车键、 向右键
};
unsigned char KeySta[4][4] = {  //全部矩阵按键的当前状态
    {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
};

void KeyDriver();

void main()
{
    EA = 1;       //使能总中断
    ENLED = 0;    //选择数码管进行显示
    ADDR3 = 1;
    TMOD = 0x01;  //设置T0为模式1
    TH0  = 0xFC;  //为T0赋初值0xFC67，定时1ms
    TL0  = 0x67;
    ET0  = 1;     //使能T0中断
    TR0  = 1;     //启动T0
    LedBuff[0] = LedChar[0];  //上电显示0
	
    while (1)
    {
        KeyDriver();   //调用按键驱动函数
    }
}
/* 将一个无符号长整型的数字显示到数码管上，num-待显示数字 */
void ShowNumber(unsigned long num)
{
    signed char i;
    unsigned char buf[6];
    
    for (i=0; i<6; i++)   //把长整型数转换为6位十进制的数组
    {
        buf[i] = num % 10;
        num = num / 10;
    }
    for (i=5; i>=1; i--)  //从最高位起，遇到0转换为空格，遇到非0则退出循环
    {
        if (buf[i] == 0)
            LedBuff[i] = 0xFF;
        else
            break;
    }
    for ( ; i>=0; i--)    //剩余低位都如实转换为数码管显示字符
    {
        LedBuff[i] = LedChar[buf[i]];
    }
}
/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
    static unsigned long result = 0;  //用于保存运算结果
    static unsigned long addend = 0;  //用于保存输入的加数
    
    if ((keycode>=0x30) && (keycode<=0x39))  //输入0-9的数字
    {
        addend = (addend*10)+(keycode-0x30); //整体十进制左移，新数字进入个位
        ShowNumber(addend);    //运算结果显示到数码管
    }
    else if (keycode == 0x26)  //向上键用作加号，执行加法或连加运算
    {
        result += addend;      //进行加法运算
        addend = 0;
        ShowNumber(result);    //运算结果显示到数码管
    }
    else if (keycode == 0x0D)  //回车键，执行加法运算(实际效果与加号相同)
    {
        result += addend;      //进行加法运算
        addend = 0;
        ShowNumber(result);    //运算结果显示到数码管
    }
    else if (keycode == 0x1B)  //Esc键，清零结果
    {
        addend = 0;
        result = 0;
        ShowNumber(addend);    //清零后的加数显示到数码管
    }
}
/* 按键驱动函数，检测按键动作，调度相应动作函数，需在主循环中调用 */
void KeyDriver()
{
    unsigned char i, j;
    static unsigned char backup[4][4] = {  //按键值备份，保存前一次的值
        {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
    };
    
    for (i=0; i<4; i++)  //循环检测4*4的矩阵按键
    {
        for (j=0; j<4; j++)
        {
            if (backup[i][j] != KeySta[i][j])    //检测按键动作
            {
                if (backup[i][j] != 0)           //按键按下时执行动作
                {
                    KeyAction(KeyCodeMap[i][j]); //调用按键动作函数
                }
                backup[i][j] = KeySta[i][j];     //刷新前一次的备份值
            }
        }
    }
}
/* 按键扫描函数，需在定时中断中调用，推荐调用间隔1ms */
void KeyScan()
{
    unsigned char i;
    static unsigned char keyout = 0;   //矩阵按键扫描输出索引
    static unsigned char keybuf[4][4] = {  //矩阵按键扫描缓冲区
        {0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF}
    };

    //将一行的4个按键值移入缓冲区
    keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
    keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
    keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
    keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;
    //消抖后更新按键状态
    for (i=0; i<4; i++)  //每行4个按键，所以循环4次
    {
        if ((keybuf[keyout][i] & 0x0F) == 0x00)
        {   //连续4次扫描值为0，即4*4ms内都是按下状态时，可认为按键已稳定的按下
            KeySta[keyout][i] = 0;
        }
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
        {   //连续4次扫描值为1，即4*4ms内都是弹起状态时，可认为按键已稳定的弹起
            KeySta[keyout][i] = 1;
        }
    }
    //执行下一次的扫描输出
    keyout++;                //输出索引递增
    keyout = keyout & 0x03;  //索引值加到4即归零
    switch (keyout)          //根据索引，释放当前输出引脚，拉低下次的输出引脚
    {
        case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
        case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
        case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
        case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
        default: break;
    }
}
/* 数码管动态扫描刷新函数，需在定时中断中调用 */
void LedScan()
{
    static unsigned char i = 0;  //动态扫描的索引
    
    P0 = 0xFF;   //显示消隐
    switch (i)
    {
        case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=LedBuff[0]; break;
        case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=LedBuff[1]; break;
        case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=LedBuff[2]; break;
        case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=LedBuff[3]; break;
        case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=LedBuff[4]; break;
        case 5: ADDR2=1; ADDR1=0; ADDR0=1; i=0; P0=LedBuff[5]; break;
        default: break;
    }
}
/* T0中断服务函数，用于数码管显示扫描与按键扫描 */
void InterruptTimer0() interrupt 1
{
    TH0 = 0xFC;  //重新加载初值
    TL0 = 0x67;
    LedScan();   //调用数码管显示扫描函数
    KeyScan();   //调用按键扫描函数
}
