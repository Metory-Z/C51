#include<reg52.h>

sbit BUZZ = P1^6;
unsigned char T0RH = 0;
unsigned char T0RL = 0;

void OpenBuzz(unsigned int frequ);
void StopBuzz();

void main()
{
    unsigned int i;

    TMOD = 0x01;
    EA = 1;
    
    buzz1 = 4000;
    num = 500;
    while (1)
    {
        for (i=buzz1; i>=0;i-num)
        {OpenBuzz(i);
        for (i=0; i<40000; i++);
        StopBuzz();
        for (i=0; i<40000; i++);
        }
    }
}

void OpenBuzz(unsigned int frequ)
{
    unsigned int reload;

    reload = 65536 - (11059200/12)/(frequ*2);////
    T0RH = (unsigned char)(reload>>8);
    T0RL = (unsigned char)reload;
    TH0 = 0xFF;
    TL0 = 0xFE;
    ET0 = 1;
    TR0 = 1;
}

void StopBuzz()
{
    ET0 = 0;
    TR0 = 0;
}

void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;
    TL0 = T0RL;
    BUZZ = ~BUZZ;// 反转蜂鸣器控制电平
}