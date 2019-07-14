/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第14章 作业题3 可修改亮灯时间的交通灯
* 版本号：v1.0.0
* 备  注：基于lesson10_4例程的基础上添加功能
*         从EEPROM地址0x10起始保存保存3种灯的亮灯时间，读取时检查其有效性
*         通过UART串口命令可以分别修改3种时间，接收命令时同样检查其有效性
*         时间设定命令为“setr xx”、“setg xx”、“sety xx” (xx为两位有效数字)
*******************************************************************************
*/

#include <reg52.h>

sbit  ADDR3 = P1^3;
sbit  ENLED = P1^4;

unsigned char code LedChar[] = {  //数码管显示字符转换表
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[7] = {  //数码管+独立LED显示缓冲区
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
bit flag1s = 1;          //1秒定时标志
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节
unsigned char RedTime, GrnTime, YlwTime; //红灯、绿灯、黄灯的设定时间

void ConfigTimer0(unsigned int ms);
void GetLightTime();
void TrafficLight();
extern void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
extern void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);

void main()
{
    EA = 1;            //开总中断
    ENLED = 0;         //使能数码管和独立LED
    ADDR3 = 1;
    ConfigTimer0(1);   //配置T0定时1ms
    ConfigUART(9600);  //配置波特率为9600
    GetLightTime();    //获取交通灯设定时间
    
    while (1)
    {
        UartDriver();  //调用串口驱动
        if (flag1s)    //每秒执行一次交通灯刷新
        {
            flag1s = 0;
            TrafficLight();
        }
    }
}
/* 从E2PROM中获取设定的灯亮时间 */
void GetLightTime()
{
    unsigned char buf[4];
    
    E2Read(buf, 0x10, 3);  //从地址0x10开始的3个字节保存三种灯的时间
    if ((buf[0]<1) || (buf[0]>99))  //检查红灯时间是否有效，有效范围1～99
        RedTime = 30;       //无效时，取默认值30
    else
        RedTime = buf[0];   //有效时，取该设定值
    
    if ((buf[1]<1) || (buf[1]>99))  //检查绿灯时间是否有效，有效范围1～99
        GrnTime = 40;       //无效时，取默认值40
    else
        GrnTime = buf[1];   //有效时，取该设定值
    
    if ((buf[2]<1) || (buf[2]>10))  //检查黄灯时间是否有效，有效范围1～10
        YlwTime = 3;        //无效时，取默认值3
    else
        YlwTime = buf[2];   //有效时，取该设定值
}
/* 设置灯亮时间，保存时间到E2并更新当前值
   str-包含设定时间数值的字符串指针，light-灯颜色索引
   返回值-时间已正确设定返回1，否则返回0 */
unsigned char SetLightTime(unsigned char *str, unsigned char light)
{
    unsigned char i;
    unsigned char tmp = 0;
    unsigned char result = 0;
    
    for (i=0; i<2; i++)  //最多取两位字符，转换为单字节数值
    {
        if ((*str >= '0') && (*str <= '9'))  //0~9为有效字符
        {
            tmp *= 10;         //先前的数据扩大10倍，即按十进制左移1位
            tmp += (*str-'0'); //当前字符数值填充到个位上
            str++;             //字符指针递增，准备下一次检测
        }
        else  //遇到无效字符即退出
        {
            break;
        }
    }
    switch (light)  //检查设定值是否有效
    {
        case 0:  //检查红灯时间
            if ((tmp>=1) && (tmp<=99))
            {
                RedTime = tmp;
                result = 1;
            }
            break;
        case 1:  //检查绿灯时间
            if ((tmp>=1) && (tmp<=99))
            {
                GrnTime = tmp;
                result = 1;
            }
            break;
        case 2:  //检查黄灯时间
            if ((tmp>=1) && (tmp<=10))
            {
                YlwTime = tmp;
                result = 1;
            }
            break;
        default:
            break;
    }
    if (result != 0)  //如果设定值有效则同时保存到E2中
    {
        E2Write(&tmp, 0x10+light, 1);
    }
    
    return result;
}

/* 交通灯显示刷新函数 */
void TrafficLight()
{
    static unsigned char color = 2;  //颜色索引：0-绿色/1-黄色/2-红色
    static unsigned char timer = 0;  //倒计时定时器
    
    if (timer == 0) //倒计时到0时，切换交通灯
    {
        switch (color)  //LED8/9代表绿灯，LED5/6代表黄灯，LED2/3代表红灯
        {
            case 0:     //切换到黄色
                color = 1;
                timer = YlwTime - 1;
                LedBuff[6] = 0xE7;
                break;
            case 1:     //切换到红色
                color = 2;
                timer = RedTime - 1;
                LedBuff[6] = 0xFC;
                break;
            case 2:     //切换到绿色
                color = 0;
                timer = GrnTime - 1;
                LedBuff[6] = 0x3F;
                break;
            default:
                break;
        }
    }
    else  //倒计时未到0时，递减其计数值
    {
        timer--;
    }
    LedBuff[0] = LedChar[timer%10];  //倒计时数值个位显示
    LedBuff[1] = LedChar[timer/10];  //倒计时数值十位显示
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
/* 串口动作函数，根据接收到的命令帧执行响应的动作
   buf-接收到的命令帧指针，len-命令帧长度 */
void UartAction(unsigned char *buf, unsigned char len)
{
    unsigned char i;
    unsigned char code cmd0[] = "setr ";  //设置红灯时间
    unsigned char code cmd1[] = "setg ";  //设置绿灯时间
    unsigned char code cmd2[] = "sety ";  //设置黄灯时间
    unsigned char code cmdLen[] = {       //命令长度汇总表
        sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,
    };
    unsigned char code *cmdPtr[] = {      //命令指针汇总表
        &cmd0[0],  &cmd1[0], &cmd2[0],
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
    if (i < sizeof(cmdLen))
    {
        buf[len] = '\0';     //为接收到的字符串添加结束符
        if (SetLightTime(buf+cmdLen[i], i) != 0)
        {
            buf[len++] = '\r';  //有效命令被执行后，在原命令帧之后添加
            buf[len++] = '\n';  //回车换行符后返回给上位机，表示已执行
            UartWrite(buf, len);
        }
        else  //未能正确设置时间时返回“错误参数”信息
        {
            UartWrite("bad parameter.\r\n", sizeof("bad parameter.\r\n")-1);
        }
    }
    else  //命令无效时返回“错误命令”信息
    {
        UartWrite("bad command.\r\n", sizeof("bad command.\r\n")-1);
    }
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
/* T0中断服务函数，执行串口接收监控和蜂鸣器驱动 */
void InterruptTimer0() interrupt 1
{
    static unsigned int tmr1s = 0;  //1秒定时器
    
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    LedScan();   //LED扫描显示
    UartRxMonitor(1);  //串口接收监控
    tmr1s++;     //1秒定时的处理
    if (tmr1s >= 1000)
    {
        tmr1s = 0;
        flag1s = 1;  //设置秒定时标志
    }
}
