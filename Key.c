#include<reg52.h>

sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;
sbit ADDR3=P1^3;
sbit ENLED=P1^4;
sbit LED9=P0^7;
sbit LED8=P0^6;
sbit LED7=P0^5;
sbit LED6=P0^4;
sbit KEY1=P2^4;
sbit KEY2=P2^5;
sbit KEY3=P2^6;
sbit KEY4=P2^7;

void main()
{
    ENLED = 0;
    ADDR3 = 1;
    ADDR2 = 1;
    ADDR1 = 1;
    ADDR0 = 0;
    P2 = 0xF7;//11110111

    while (1)
    {
        LED9 = KEY1;// 按键按下，按键输入引脚为0，赋值给LED
        LED8 = KEY2;
        LED7 = KEY3;
        LED6 = KEY4;
    } 
}
