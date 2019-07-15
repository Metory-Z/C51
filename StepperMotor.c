#include<reg52.h>

unsigned char code BeatCode[8] = {
    0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
};

void delay();

void main()
{
    unsigned char tmp, index = 0;
    
    while (1)
    {
        tmp = P1;// 暂存P1 8个口的值
        tmp = tmp & 0xF0;// 低
        tmp = tmp | BeatCode[index];
        P1 = tmp;
        index++;
        index = index & 0x07;// 到8归零
        delay();
    }
}

void delay()
{
    unsigned int i = 200;

    while (i--);
}