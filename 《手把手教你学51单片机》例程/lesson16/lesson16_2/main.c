/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第16章 温度传感器DS18B20示例
* 版本号：v1.0.0
* 备  注：详情见第16章16.4节
*         控制DS18B20测量温度并将温度值显示到液晶上
*******************************************************************************
*/

#include <reg52.h>

bit flag1s = 0;          //1s定时标志
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

void ConfigTimer0(unsigned int ms);
unsigned char IntToString(unsigned char *str, int dat);
extern bit Start18B20();
extern bit Get18B20Temp(int *temp);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

void main()
{
    bit res;
    int temp;        //读取到的当前温度值
    int intT, decT;  //温度值的整数和小数部分
    unsigned char len;
    unsigned char str[12];

    EA = 1;            //开总中断
    ConfigTimer0(10);  //T0定时10ms
    Start18B20();      //启动DS18B20
    InitLcd1602();     //初始化液晶
    
    while (1)
    {
        if (flag1s)  //每秒更新一次温度
        {
            flag1s = 0;
            res = Get18B20Temp(&temp);  //读取当前温度
            if (res)                    //读取成功时，刷新当前温度显示
            {
                intT = temp >> 4;             //分离出温度值整数部分
                decT = temp & 0xF;            //分离出温度值小数部分
                len = IntToString(str, intT); //整数部分转换为字符串
                str[len++] = '.';             //添加小数点
                decT = (decT*10) / 16;        //二进制的小数部分转换为1位十进制位
                str[len++] = decT + '0';      //十进制小数位再转换为ASCII字符
                while (len < 6)               //用空格补齐到6个字符长度
                {
                    str[len++] = ' ';
                }
                str[len] = '\0';              //添加字符串结束符
                LcdShowStr(0, 0, str);        //显示到液晶屏上
            }
            else                        //读取失败时，提示错误信息
            {
                LcdShowStr(0, 0, "error!");
            }
            Start18B20();               //重新启动下一次转换
        }
    }
}
/* 整型数转换为字符串，str-字符串指针，dat-待转换数，返回值-字符串长度 */
unsigned char IntToString(unsigned char *str, int dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[6];
    
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
/* T0中断服务函数，完成1秒定时 */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr1s = 0;
    
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    tmr1s++;
    if (tmr1s >= 100)  //定时1s
    {
        tmr1s = 0;
        flag1s = 1;
    }
}
