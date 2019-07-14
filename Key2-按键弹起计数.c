#include<reg52.h>

sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;
sbit ADDR3=P1^3;
sbit ENLED=P1^4;
sbit KEY1=P2^4;
sbit KEY2=P2^5;
sbit KEY3=P2^6;
sbit KEY4=P2^7;

unsigned char code LedChar[] =
    {
	    0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,
	    0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E
	};

void main()
{
    bit backup = 1;// 位变量
    unsigned char cnt = 0;

    ENLED = 0;
    ADDR3 = 1;
    ADDR2 = 1;
    ADDR1 = 0;
    ADDR0 = 0;
    P2 = 0xF7;
    P0 = LedChar[cnt];

    while (1)
    {
        if (KEY4 != backup)
        {
            if (backup == 0)
            {
                cnt++;
                if (cnt >= 10)
                {
                    cnt = 0;
                }
                P0 = LedChar[cnt];
            }
            backup = KEY4;
        }
    } 
}
