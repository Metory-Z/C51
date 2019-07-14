/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第13章 基于1602液晶的简易计算器示例
* 版本号：v1.0.0
* 备  注：详情见第13章13.4节
*         实现整型数为操作数的加减乘除计算器
*******************************************************************************
*/

#include <reg52.h>

unsigned char step = 0;  //操作步骤
unsigned char oprt = 0;  //运算类型
signed long num1 = 0;    //操作数1
signed long num2 = 0;    //操作数2
signed long result = 0;  //运算结果
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

void ConfigTimer0(unsigned int ms);
extern void KeyScan();
extern void KeyDriver();
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdAreaClear(unsigned char x, unsigned char y, unsigned char len);
extern void LcdFullClear();

void main()
{
    EA = 1;           //开总中断
    ConfigTimer0(1);  //配置T0定时1ms
	InitLcd1602();    //初始化液晶
    LcdShowStr(15, 1, "0");  //初始显示一个数字0
    
    while (1)
    {
        KeyDriver();  //调用按键驱动
    }
}
/* 长整型数转换为字符串，str-字符串指针，dat-待转换数，返回值-字符串长度 */
unsigned char LongToString(unsigned char *str, signed long dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[12];
    
    if (dat < 0)  //如果为负数，首先取绝对值，并在指针上添加负号
    {
        dat = -dat;
        *str++ = '-';
        len++;
    }
    do {          //先转换为低位在前的十进制数组
        buf[i++] = dat % 10;
        dat /= 10;
    } while (dat > 0);
    len += i;     //i最后的值就是有效字符的个数
    while (i-- > 0)   //将数组值转换为ASCII码反向拷贝到接收指针上
    {
        *str++ = buf[i] + '0';
    }
    *str = '\0';  //添加字符串结束符
    
    return len;   //返回字符串长度
}
/* 显示运算符，显示位置y，运算符类型type */
void ShowOprt(unsigned char y, unsigned char type)
{
    switch (type)
    {
        case 0: LcdShowStr(0, y, "+"); break;  //0代表+
        case 1: LcdShowStr(0, y, "-"); break;  //1代表-
        case 2: LcdShowStr(0, y, "*"); break;  //2代表*
        case 3: LcdShowStr(0, y, "/"); break;  //3代表/
        default: break;
    }
}
/* 计算器复位，清零变量值，清除屏幕显示 */
void Reset()
{
    num1 = 0;
    num2 = 0;
    step = 0;
    LcdFullClear();
}
/* 数字键动作函数，n-按键输入的数值 */
void NumKeyAction(unsigned char n)
{
    unsigned char len;
    unsigned char str[12];
    
    if (step > 1)  //如计算已完成，则重新开始新的计算
    {
        Reset();
    }
    if (step == 0)  //输入第一操作数
    {
        num1 = num1*10 + n;             //输入数值累加到原操作数上
        len = LongToString(str, num1);  //新数值转换为字符串
        LcdShowStr(16-len, 1, str);     //显示到液晶第二行上
    }
    else            //输入第二操作数
    {
        num2 = num2*10 + n;             //输入数值累加到原操作数上
        len = LongToString(str, num2);  //新数值转换为字符串
        LcdShowStr(16-len, 1, str);     //显示到液晶第二行上
    }
}
/* 运算符按键动作函数，运算符类型type */
void OprtKeyAction(unsigned char type)
{
    unsigned char len;
    unsigned char str[12];
    
    if (step == 0)  //第二操作数尚未输入时响应，即不支持连续操作
    {
        len = LongToString(str, num1); //第一操作数转换为字符串
        LcdAreaClear(0, 0, 16-len);    //清除第一行左边的字符位
        LcdShowStr(16-len, 0, str);    //字符串靠右显示在第一行
        ShowOprt(1, type);             //在第二行显示操作符
        LcdAreaClear(1, 1, 14);        //清除第二行中间的字符位
        LcdShowStr(15, 1, "0");        //在第二行最右端显示0
        oprt = type;                   //记录操作类型
        step = 1;
    }
}
/* 计算结果函数 */
void GetResult()
{
    unsigned char len;
    unsigned char str[12];
    
    if (step == 1) //第二操作数已输入时才执行计算
    {
        step = 2;
        switch (oprt)  //根据运算符类型计算结果，未考虑溢出问题
        {
            case 0: result = num1 + num2; break;
            case 1: result = num1 - num2; break;
            case 2: result = num1 * num2; break;
            case 3: result = num1 / num2; break;
            default: break;
        }
        len = LongToString(str, num2);  //原第二操作数和运算符显示到第一行
        ShowOprt(0, oprt);
        LcdAreaClear(1, 0, 16-1-len);
        LcdShowStr(16-len, 0, str);
        len = LongToString(str, result);  //计算结果和等号显示在第二行
        LcdShowStr(0, 1, "=");
        LcdAreaClear(1, 1, 16-1-len);
        LcdShowStr(16-len, 1, str);
    }
}
/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
    if  ((keycode>='0') && (keycode<='9'))  //输入字符
    {
        NumKeyAction(keycode - '0');
    }
    else if (keycode == 0x26)  //向上键，+
    {
        OprtKeyAction(0);
    }
    else if (keycode == 0x28)  //向下键，-
    {
        OprtKeyAction(1);
    }
    else if (keycode == 0x25)  //向左键，*
    {
        OprtKeyAction(2);
    }
    else if (keycode == 0x27)  //向右键，÷
    {
        OprtKeyAction(3);
    }
    else if (keycode == 0x0D)  //回车键，计算结果
    {
        GetResult();
    }
    else if (keycode == 0x1B)  //Esc键，清除
    {
        Reset();
        LcdShowStr(15, 1, "0");
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
/* T0中断服务函数，执行按键扫描 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    KeyScan();   //按键扫描
}
