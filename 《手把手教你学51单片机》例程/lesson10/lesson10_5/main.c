/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第10章 长短按键/连续按键功能实现示例
* 版本号：v1.0.0
* 备  注：详情见第10章10.5节
*******************************************************************************
*/

#include <reg52.h>

sbit BUZZ  = P1^6;
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
unsigned char LedBuff[7] = {  //数码管+独立LED显示缓冲区
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
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
unsigned long pdata KeyDownTime[4][4] = {  //每个按键按下的持续时间，单位ms
    {0, 0, 0, 0},  {0, 0, 0, 0},  {0, 0, 0, 0},  {0, 0, 0, 0}
};
bit enBuzz = 0;     //蜂鸣器使能标志
bit flag1s = 0;     //1秒定时标志
bit flagStart = 0;  //倒计时启动标志
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节
unsigned int  CountDown = 0;  //倒计时计数器

void ConfigTimer0(unsigned int ms);
void ShowNumber(unsigned long num);
void KeyDriver();

void main()
{
    EA = 1;       //使能总中断
    ENLED = 0;    //选择数码管和独立LED
    ADDR3 = 1;
    ConfigTimer0(1);  //配置T0定时1ms
    ShowNumber(0);    //上电显示0
	
    while (1)
    {
        KeyDriver();  //调用按键驱动函数
        if (flagStart && flag1s) //倒计时启动且1秒定时到达时，处理倒计时
        {
            flag1s = 0;
            if (CountDown > 0)   //倒计时未到0时，计数器递减
            {
                CountDown--;
                ShowNumber(CountDown); //刷新倒计时数显示
                if (CountDown == 0)    //减到0时，执行声光报警
                {
                    enBuzz = 1;        //启动蜂鸣器发声
                    LedBuff[6] = 0x00; //点亮独立LED
                }
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
void KeyAction(unsigned char keycode)  //按键动作函数，根据键码执行相应动作
{
    if (keycode == 0x26)       //向上键，倒计时设定值递增
    {
        if (CountDown < 9999)  //最大计时9999秒
        {
            CountDown++;
            ShowNumber(CountDown);
        }
    }
    else if (keycode == 0x28)  //向下键，倒计时设定值递减
    {
        if (CountDown > 1)     //最小计时1秒
        {
            CountDown--;
            ShowNumber(CountDown);
        }
    }
    else if (keycode == 0x0D)  //回车键，启动倒计时
    {
        flagStart = 1;         //启动倒计时
    }
    else if (keycode == 0x1B)  //Esc键，取消倒计时
    {
        enBuzz = 0;            //关闭蜂鸣器
        LedBuff[6] = 0xFF;     //关闭独立LED
        flagStart = 0;         //停止倒计时
        CountDown = 0;         //倒计时数归零
        ShowNumber(CountDown);
    }
}
/* 按键驱动函数，检测按键动作，调度相应动作函数，需在主循环中调用 */
void KeyDriver()
{
    unsigned char i, j;
    static unsigned char pdata backup[4][4] = {  //按键值备份，保存前一次的值
        {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
    };
    static unsigned long pdata TimeThr[4][4] = {  //快速输入执行的时间阈值
        {1000, 1000, 1000, 1000},  {1000, 1000, 1000, 1000},
        {1000, 1000, 1000, 1000},  {1000, 1000, 1000, 1000}
    };
    
    for (i=0; i<4; i++)  //循环扫描4*4的矩阵按键
    {
        for (j=0; j<4; j++)
        {
            if (backup[i][j] != KeySta[i][j])     //检测按键动作
            {
                if (backup[i][j] != 0)            //按键按下时执行动作
                {
                    KeyAction(KeyCodeMap[i][j]);  //调用按键动作函数
                }
                backup[i][j] = KeySta[i][j];      //刷新前一次的备份值
            }
            if (KeyDownTime[i][j] > 0)            //检测执行快速输入
            {
                if (KeyDownTime[i][j] >= TimeThr[i][j])
                {                                 //达到阈值时执行一次动作
                    KeyAction(KeyCodeMap[i][j]);  //调用按键动作函数
                    TimeThr[i][j] += 200; //时间阈值增加200ms，以准备下次执行
                }
            }
            else   //按键弹起时复位阈值时间
            {
                TimeThr[i][j] = 1000;  //恢复1s的初始阈值时间
            }
        }
    }
}
/* 按键扫描函数，需在定时中断中调用 */
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
            KeyDownTime[keyout][i] += 4;  //按下的持续时间累加
        }
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
        {   //连续4次扫描值为1，即4*4ms内都是弹起状态时，可认为按键已稳定的弹起
            KeySta[keyout][i] = 1;
            KeyDownTime[keyout][i] = 0;   //按下的持续时间清零
        }
    }
    //执行下一次的扫描输出
    keyout++;        //输出索引递增
    keyout &= 0x03;  //索引值加到4即归零
    switch (keyout)  //根据索引，释放当前输出引脚，拉低下次的输出引脚
    {
        case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
        case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
        case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
        case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
        default: break;
    }
}
/* LED动态扫描刷新函数，需在定时中断中调用 */
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
/* T0中断服务函数，完成数码管、按键扫描与秒定时 */
void InterruptTimer0() interrupt 1
{
    static unsigned int tmr1s = 0;  //1秒定时器
    
    TH0 = T0RH;   //重新加载重载值
    TL0 = T0RL;
    if (enBuzz)   //蜂鸣器发声处理
        BUZZ = ~BUZZ;  //驱动蜂鸣器发声
    else
        BUZZ = 1;      //关闭蜂鸣器
    LedScan();   //LED扫描显示
    KeyScan();   //按键扫描
    if (flagStart)  //倒计时启动时处理1秒定时
    {
        tmr1s++;
        if (tmr1s >= 1000)
        {
            tmr1s = 0;
            flag1s = 1;
        }
    }
    else  //倒计时未启动时1秒定时器始终归零
    {
        tmr1s = 0;
    }
}

