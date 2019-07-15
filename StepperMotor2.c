#include<reg52.h>

void TurnMortor(unsigned long angle);

void main()
{
    TurnMortor(360 * 25);
    while(1);
}

void delay()
{
    unsigned int i = 200;

    while (i--);
}

unsigned char code BeatCode[8] = {
    0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
};

void TurnMortor(unsigned long angle)
{
    unsigned char tmp, index = 0;
    unsigned long beats = 0;// 所需节拍数
    unsigned char code BeatCode[8] = {
        0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
    };

    beats = (angle * 4096) / 360;// 一圈是4096 拍
    while (beats--)
    {
        tmp = P1;
        tmp = tmp & 0xF0;
        tmp = tmp | BeatCode[index];
        P1 = tmp;
        index++;
        index = index & 0x07;
        delay();
    }
    P1 = P1 | 0x0F;
}