/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第18章 基于RS485的Modbus通信示例
* 版本号：v1.0.0
* 备  注：详情见第18章18.3节
*         支持Modbus协议的寄存器读取和单寄存器写入操作
*******************************************************************************
*/

#include <reg52.h>

sbit BUZZ = P1^6;

bit flagBuzzOn = 0;   //蜂鸣器启动标志
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节
unsigned char regGroup[5];  //Modbus寄存器组，地址为0x00～0x04

void ConfigTimer0(unsigned int ms);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);
extern unsigned int GetCRC16(unsigned char *ptr,  unsigned char len);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

void main()
{
    EA = 1;            //开总中断
    ConfigTimer0(1);   //配置T0定时1ms
    ConfigUART(9600);  //配置波特率为9600
    InitLcd1602();     //初始化液晶
    
    while (1)
    {
        UartDriver();  //调用串口驱动
    }
}
/* 串口动作函数，根据接收到的命令帧执行响应的动作
   buf-接收到的命令帧指针，len-命令帧长度 */
void UartAction(unsigned char *buf, unsigned char len)
{
    unsigned char i;
    unsigned char cnt;
    unsigned char str[4];
    unsigned int  crc;
    unsigned char crch, crcl;
    
    if (buf[0] != 0x01) //本例中的本机地址设定为0x01，
    {                   //如数据帧中的地址字节与本机地址不符，
        return;         //则直接退出，即丢弃本帧数据不做任何处理
    }
    //地址相符时，再对本帧数据进行校验
    crc = GetCRC16(buf, len-2);  //计算CRC校验值
    crch = crc >> 8;
    crcl = crc & 0xFF;
    if ((buf[len-2]!=crch) || (buf[len-1]!=crcl))
    {
        return;   //如CRC校验不符时直接退出
    }
    //地址和校验字均相符后，解析功能码，执行相关操作
    switch (buf[1])
    {
        case 0x03:  //读取一个或连续的寄存器
            if ((buf[2]==0x00) && (buf[3]<=0x05)) //只支持0x0000～0x0005
            {
                if (buf[3] <= 0x04)
                {
                    i = buf[3];      //提取寄存器地址
                    cnt = buf[5];    //提取待读取的寄存器数量
                    buf[2] = cnt*2;  //读取数据的字节数，为寄存器数*2
                    len = 3;         //帧前部已有地址、功能码、字节数共3个字节
                    while (cnt--)
                    {
                        buf[len++] = 0x00;          //寄存器高字节补0
                        buf[len++] = regGroup[i++]; //寄存器低字节
                    }
                }
                else  //地址0x05为蜂鸣器状态
                {
                    buf[2] = 2;  //读取数据的字节数
                    buf[3] = 0x00;
                    buf[4] = flagBuzzOn;
                    len = 5;
                }
                break;
            }
            else  //寄存器地址不被支持时，返回错误码
            {
                buf[1] = 0x83;  //功能码最高位置1
                buf[2] = 0x02;  //设置异常码为02-无效地址
                len = 3;
                break;
            }
            
        case 0x06:  //写入单个寄存器
            if ((buf[2]==0x00) && (buf[3]<=0x05)) //只支持0x0000～0x0005
            {
                if (buf[3] <= 0x04)
                {
                    i = buf[3];             //提取寄存器地址
                    regGroup[i] = buf[5];   //保存寄存器数据
                    cnt = regGroup[i] >> 4; //显示到液晶上
                    if (cnt >= 0xA)
                        str[0] = cnt - 0xA + 'A';
                    else
                        str[0] = cnt + '0';
                    cnt = regGroup[i] & 0x0F;
                    if (cnt >= 0xA)
                        str[1] = cnt - 0xA + 'A';
                    else
                        str[1] = cnt + '0';
                    str[2] = '\0';
                    LcdShowStr(i*3, 0, str);
                }
                else  //地址0x05为蜂鸣器状态
                {
                    flagBuzzOn = (bit)buf[5]; //寄存器值转为蜂鸣器的开关
                }
                len -= 2; //长度-2以重新计算CRC并返回原帧
                break;
            }
            else  //寄存器地址不被支持时，返回错误码
            {
                buf[1] = 0x86;  //功能码最高位置1
                buf[2] = 0x02;  //设置异常码为02-无效地址
                len = 3;
                break;
            }
            
        default:  //其它不支持的功能码
            buf[1] |= 0x80;  //功能码最高位置1
            buf[2] = 0x01;   //设置异常码为01-无效功能
            len = 3;
            break;
    }
    crc = GetCRC16(buf, len); //计算返回帧的CRC校验值
    buf[len++] = crc >> 8;    //CRC高字节
    buf[len++] = crc & 0xFF;  //CRC低字节
    UartWrite(buf, len);      //发送返回帧
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
    if (flagBuzzOn)  //执行蜂鸣器鸣叫或关闭
        BUZZ = ~BUZZ;
    else
        BUZZ = 1;
    UartRxMonitor(1);  //串口接收监控
}
