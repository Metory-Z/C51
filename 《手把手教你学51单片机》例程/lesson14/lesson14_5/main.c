/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第14章 简易电子广告屏示例
* 版本号：v1.0.0
* 备  注：详情见第14章14.4节
*         读取EEPROM中的数据显示到液晶，并可通过UART修改EEPROM中的数据
*******************************************************************************
*/

#include <reg52.h>

unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

void InitShowStr();
void ConfigTimer0(unsigned int ms);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
extern void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);

void main()
{
    EA = 1;            //开总中断
    ConfigTimer0(1);   //配置T0定时1ms
    ConfigUART(9600);  //配置波特率为9600
    InitLcd1602();     //初始化液晶
    InitShowStr();     //初始显示内容
    
    while (1)
    {
        UartDriver();  //调用串口驱动
    }
}
/* 处理液晶屏初始显示内容 */
void InitShowStr()
{
    unsigned char str[17];
    
    str[16] = '\0';         //在最后添加字符串结束符，确保字符串可以结束
    E2Read(str, 0x20, 16);  //读取第一行字符串，其E2起始地址为0x20
    LcdShowStr(0, 0, str);  //显示到液晶屏
    E2Read(str, 0x40, 16);  //读取第二行字符串，其E2起始地址为0x40
    LcdShowStr(0, 1, str);  //显示到液晶屏
}
/* 内存比较函数，比较两个指针所指向的内存数据是否相同，
   ptr1-待比较指针1，ptr2-待比较指针2，len-待比较长度
   返回值-两段内存数据完全相同时返回1，不同返回0 */
bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2, unsigned char len)
{
    while (len--)
    {
        if (*ptr1++ != *ptr2++)  //遇到不相等数据时即刻返回0
        {
            return 0;
        }
    }
    return 1;  //比较完全部长度数据都相等则返回1
}
/* 将一字符串整理成16字节的固定长度字符串，不足部分补空格
   out-整理后的字符串输出指针，in-待整理字符串指针 */
void TrimString16(unsigned char *out, unsigned char *in)
{
    unsigned char i = 0;
    
    while (*in != '\0')  //拷贝字符串直到输入字符串结束
    {
        *out++ = *in++;
        i++;
        if (i >= 16)    //当拷贝长度已达到16字节时，强制跳出循环
        {
            break;
        }
    }
    for ( ; i<16; i++)  //如不足16个字节则用空格补齐
    {
        *out++ = ' ';
    }
    *out = '\0';        //最后添加结束符
}
/* 串口动作函数，根据接收到的命令帧执行响应的动作
   buf-接收到的命令帧指针，len-命令帧长度 */
void UartAction(unsigned char *buf, unsigned char len)
{
    unsigned char i;
    unsigned char str[17];
    unsigned char code cmd0[] = "showstr1 ";  //第一行字符显示命令
    unsigned char code cmd1[] = "showstr2 ";  //第二行字符显示命令
    unsigned char code cmdLen[] = {           //命令长度汇总表
        sizeof(cmd0)-1, sizeof(cmd1)-1,
    };
    unsigned char code *cmdPtr[] = {          //命令指针汇总表
        &cmd0[0],  &cmd1[0],
    };

    for (i=0; i<sizeof(cmdLen); i++)  //遍历命令列表，查找相同命令
    {
        if (len >= cmdLen[i])  //首先接收到的数据长度要不小于命令长度
        {
            if (CmpMemory(buf, cmdPtr[i], cmdLen[i]))  //比较相同时退出循环
            {
                break;
            }
        }
    }
    switch (i) //根据比较结果执行相应命令
    {
        case 0:
            buf[len] = '\0';                   //为接收到的字符串添加结束符
            TrimString16(str, buf+cmdLen[0]);  //整理成16字节固定长度字符串
            LcdShowStr(0, 0, str);             //显示字符串1
            E2Write(str, 0x20, sizeof(str));   //保存字符串1，起始地址为0x20
            break;
        case 1:
            buf[len] = '\0';                   //为接收到的字符串添加结束符
            TrimString16(str, buf+cmdLen[1]);  //整理成16字节固定长度字符串
            LcdShowStr(0, 1, str);             //显示字符串1
            E2Write(str, 0x40, sizeof(str));   //保存字符串2，起始地址为0x40
            break;
        default:   //未找到相符命令时，给上机发送“错误命令”的提示
            UartWrite("bad command.\r\n", sizeof("bad command.\r\n")-1);
            return;
    }
    buf[len++] = '\r';  //有效命令被执行后，在原命令帧之后添加
    buf[len++] = '\n';  //回车换行符后返回给上位机，表示已执行
    UartWrite(buf, len);
}
/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 33;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}
/* T0中断服务函数，执行串口接收监控和蜂鸣器驱动 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    UartRxMonitor(1);  //串口接收监控
}
