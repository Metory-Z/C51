#include<reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code LedChar[] =
    {
	    0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,
	    0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E
	};
unsigned char LedStart[6];

void main()
{
    unsigned int cnt = 0, sec = 0, i = 0;
    ENLED=0;
	ADDR3=1;
    TMOD=0x01;
	TH0=0xF5;
	TL0=0x32; // 3ms
	TR0=1;

    while(1)
    {
        if (TF0 == 1)
        {
            TF0 = 0;
            TH0=0xF5;
	        TL0=0x32;
            cnt++;
            if (cnt >= 334)
            {
                cnt = 0;
                sec++;
            }
            if(i==0)
			{ADDR2=0;ADDR1=0;ADDR0=0;i++;P0=LedChar[sec%10];}
			else if(i==1)
			{ADDR2=0;ADDR1=0;ADDR0=1;i++;P0=LedChar[sec/10%10];}
			else if(i==2)
			{ADDR2=0;ADDR1=1;ADDR0=0;i++;P0=LedChar[sec/100%10];}
			else if(i==3)
			{ADDR2=0;ADDR1=1;ADDR0=1;i++;P0=LedChar[sec/1000%10];}
			else if(i==4)
			{ADDR2=1;ADDR1=0;ADDR0=0;i++;P0=LedChar[sec/10000%10];}
			else if(i==5)
			{ADDR2=1;ADDR1=0;ADDR0=1;i=0;P0=LedChar[sec/100000%10];}
    		// 3ms 计算一次
		}
    }
}
