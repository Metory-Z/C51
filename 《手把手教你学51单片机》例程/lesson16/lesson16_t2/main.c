/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第16章 作业题2 红外遥控器控制电机转动
* 版本号：v1.0.0
* 备  注：用遥控器上的加号和减号键分别控制步进电机正转或反转一周。
*         不同遥控器的键码值可能不同，如需确认每个键的键码值可使用lesson16_1的
*         示例程序（数码管右端显示的数值即是键码值，十六进制格式显示）。
*******************************************************************************
*/

#include <reg52.h>

signed long beats = 0;  //电机转动节拍总数
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

extern bit irflag;
extern unsigned char ircode[4];
extern void InitInfrared();
void ConfigTimer0(unsigned int ms);
void StartMotor(signed long angle);
void StopMotor();

void main()
{
    EA = 1;      //开总中断
    InitInfrared();   //初始化红外功能
    ConfigTimer0(2);  //配置T0定时2ms
    
    while (1)
    {
        if (irflag)  //接收到红外数据时刷新显示
        {
            irflag = 0;
            if (ircode[2] == 0x09)  //0x09为+键码，正转1圈
            {
                StartMotor(360);
            }
            else if (ircode[2] == 0x15)  //0x15为-键码，反转1圈
            {
                StartMotor(-360);
            }
        }
    }
}
/* 步进电机启动函数，angle-需转过的角度 */
void StartMotor(signed long angle)
{
    //在计算前关闭中断，完成后再打开，以避免中断打断计算过程而造成错误
    EA = 0;
    beats = (angle * 4076) / 360; //实测为4076拍转动一圈
    EA = 1;
}
/* 步进电机停止函数 */
void StopMotor()
{
    EA = 0;
    beats = 0;
    EA = 1;
}
/* 电机转动控制函数 */
void TurnMotor()
{
    unsigned char tmp;  //临时变量
    static unsigned char index = 0;  //节拍输出索引
    unsigned char code BeatCode[8] = {  //步进电机节拍对应的IO控制代码
        0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
    };
    
    if (beats != 0)  //节拍数不为0则产生一个驱动节拍
    {
        if (beats > 0)  //节拍数大于0时正转
        {
            index++;               //正转时节拍输出索引递增
            index = index & 0x07;  //用&操作实现到8归零
            beats--;               //正转时节拍计数递减
        }
        else            //节拍数小于0时反转
        {
            index--;               //反转时节拍输出索引递减
            index = index & 0x07;  //用&操作同样可以实现到-1时归7
            beats++;               //反转时节拍计数递增
        }
        tmp = P1;                    //用tmp把P1口当前值暂存
        tmp = tmp & 0xF0;            //用&操作清零低4位
        tmp = tmp | BeatCode[index]; //用|操作把节拍代码写到低4位
        P1  = tmp;                   //把低4位的节拍代码和高4位的原值送回P1
    }
    else  //节拍数为0则关闭电机所有的相
    {
        P1 = P1 | 0x0F;
    }
}
/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 13;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}
/* T0中断服务函数，执行数码管扫描显示 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    TurnMotor(); //执行电机驱动
}
