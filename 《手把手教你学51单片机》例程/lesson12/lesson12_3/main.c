/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第12章 指针、字符串、字符数组、ASCII码演示示例
* 版本号：v1.0.0
* 备  注：详情见第12章12.3节
*******************************************************************************
*/

#include <reg52.h>

bit cmdArrived = 0;   //命令到达标志，即接收到上位机下发的命令
unsigned char cmdIndex = 0; //命令索引，即与上位机约定好的数组编号
unsigned char cntTxd = 0;   //串口发送计数器
unsigned char *ptrTxd;      //串口发送指针

unsigned char array1[] = "1-Hello!\r\n";
unsigned char array2[] = {'2', '-', 'H', 'e', 'l', 'l', 'o', '!', '\r', '\n'};
unsigned char array3[] = {51,  45,  72,  101, 108, 108, 111, 33,  13,   10};
unsigned char array4[] = "4-Hello!\r\n";

void ConfigUART(unsigned int baud);

void main()
{
    EA = 1;  //开总中断
    ConfigUART(9600);  //配置波特率为9600
    
    while (1)
    {
        if (cmdArrived)
        {
            cmdArrived = 0;
            switch (cmdIndex)
            {
                case 1:
                    ptrTxd = array1;         //数组1的首地址赋值给发送指针
                    cntTxd = sizeof(array1); //数组1的长度赋值给发送计数器
                    TI = 1;   //手动方式启动发送中断，处理数据发送
                    break;
                case 2:
                    ptrTxd = array2;
                    cntTxd = sizeof(array2);
                    TI = 1;
                    break;
                case 3:
                    ptrTxd = array3;
                    cntTxd = sizeof(array3);
                    TI = 1;
                    break;
                case 4:
                    ptrTxd = array4;
                    cntTxd = sizeof(array4) - 1; //字符串实际长度为数组长度减1
                    TI = 1;
                    break;
                default:
                    break;
            }
        }
    }
}
/* 串口配置函数，baud-通信波特率 */
void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //配置串口为模式1
    TMOD &= 0x0F;  //清零T1的控制位
    TMOD |= 0x20;  //配置T1为模式2
    TH1 = 256 - (11059200/12/32)/baud;  //计算T1重载值
    TL1 = TH1;     //初值等于重载值
    ET1 = 0;       //禁止T1中断
    ES  = 1;       //使能串口中断
    TR1 = 1;       //启动T1
}
/* UART中断服务函数 */
void InterruptUART() interrupt 4
{
    if (RI)  //接收到字节
    {
        RI = 0;  //清零接收中断标志位
        cmdIndex = SBUF;  //接收到的数据保存到命令索引中
        cmdArrived = 1;   //设置命令到达标志
    }
    if (TI)  //字节发送完毕
    {
        TI = 0;  //清零发送中断标志位
        if (cntTxd > 0)  //有待发送数据时，继续发送后续字节
        {
            SBUF = *ptrTxd;  //发出指针指向的数据
            cntTxd--;        //发送计数器递减
            ptrTxd++;        //发送指针递增
        }
    }
}
