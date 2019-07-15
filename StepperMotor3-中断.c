#include<reg52.h>

unsigned long beats = 0;

void TurnMortor(unsigned long angle);

void main()
{
    EA = 1;
    TMOD = 0X01;
    TH0 = 0XF8;
    TL0 = 0XCD;// 2ms
    ET0 = 1;
    TR0 = 1;

    StartMotor(360 * 2 + 180);
    while (1);// 用来等待中断的产生
}

void StartMotor(unsigned long angle)
{
    EA = 0;// 关闭中断，避免打断运算过程
    beats = (angle * 4076) / 360;
    EA = 1;
}

void InterruptTimer0() intertupt 1
{
    unsigned char tmp
    static unsigned char index = 0;
    unsigned char code BeatCode[8] = {
        0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
    };

    TH0 = 0XF8;
    TL0 = 0XCD;
    if (beats != 0)
    {
        tmp = P1;
        tmp = tmp & 0xF0;
        tmp = tmp | BeatCode[index];
        P1 = tmp;
        index++;
        index = index & 0x07;
        beats--;
    }
    else
        P1 = P1 | 0x0F;
}