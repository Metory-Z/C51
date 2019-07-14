/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第18章 作业题3 Modbus协议实现校时
* 版本号：v1.0.0
* 备  注：
* 1、在lesson15_3的基础上去掉按键校时，添加lesson18_2中的Modbus协议支持
* 2、利用Modbus调试精灵的写寄存器功能，可修改日期时间的每一个字节
* 3、寄存器地址0x0000～0x0006分别对应“年/月/日/时/分/秒/星期”
* 4、RS485方向控制信号由原来的P1.7改为P2.0，因本例使用了DS1302而未使用按键
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
bit reqRefresh = 0;  //时间刷新请求
struct sTime bufTime;  //日期时间缓冲区
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

void ConfigTimer0(unsigned int ms);
void RefreshTimeShow();
extern void InitDS1302();
extern void GetRealTime(struct sTime *time);
extern void SetRealTime(struct sTime *time);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);
extern unsigned int GetCRC16(unsigned char *ptr,  unsigned char len);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

void main()
{
    unsigned char psec=0xAA;  //秒备份，初值AA确保首次读取时间后会刷新显示

    EA = 1;            //开总中断
    ConfigTimer0(1);   //配置T0定时1ms
    ConfigUART(9600);  //配置波特率为9600
    InitDS1302();      //初始化实时时钟
    InitLcd1602();     //初始化液晶
    
    //初始化屏幕上固定不变的内容
    LcdShowStr(3, 0, "20  -  -  ");
    LcdShowStr(4, 1, "  :  :  ");
    
    while (1)
    {
        UartDriver();  //调用串口驱动
        if (flag200ms)
        {
            flag200ms = 0;
            GetRealTime(&bufTime);    //获取当前时间
            if (reqRefresh || (psec!=bufTime.sec))
            {   //检测到时间刷新请求或时间有变化时刷新显示
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
/* 串口动作函数，根据接收到的命令帧执行响应的动作
   buf-接收到的命令帧指针，len-命令帧长度 */
void UartAction(unsigned char *buf, unsigned char len)
{
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
        case 0x06:  //写入单个寄存器
            if ((buf[2]==0x00) && (buf[3]<=0x06)) //地址0x0000～0x0006分别对应
            {                                     // “年/月/日/时/分/秒/星期”
                GetRealTime(&bufTime);  //获取当前时间
                switch (buf[3])  //由寄存器地址决定要修改的时间位
                {
                    case 0: bufTime.year = 0x2000 + buf[5]; break;
                    case 1: bufTime.mon  = buf[5]; break;
                    case 2: bufTime.day  = buf[5]; break;
                    case 3: bufTime.hour = buf[5]; break;
                    case 4: bufTime.min  = buf[5]; break;
                    case 5: bufTime.sec  = buf[5]; break;
                    case 6: bufTime.week = buf[5]; break;
                    default: break;
                }
                SetRealTime(&bufTime);  //写入新修改后的时间
                reqRefresh = 1;  //设置显示刷新请求
                len -= 2;  //长度-2以重新计算CRC并返回原帧
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
/* T0中断服务函数，执行按键扫描和200ms定时 */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr200ms = 0;
    
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    UartRxMonitor(1);  //串口接收监控
    tmr200ms++;
    if (tmr200ms >= 200)  //定时200ms
    {
        tmr200ms = 0;
        flag200ms = 1;
    }
}
