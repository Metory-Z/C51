#include<reg52.h>

sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;
sbit ADDR3=P1^3;
sbit ENLED=P1^4;

unsigned char code LedChar[]={
	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,
	0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E
	};

void main()
{
	unsigned char cnt=0;
	unsigned char sec=0;
	
	ENLED=0;
	ADDR3=1;
	ADDR2=0;
	ADDR1=0;
	ADDR0=0;
	TMOD=0x01;
	TH0=0xB8;
	TL0=0x00;
	TR0=1;
	
	while(1)
	{
		if(TF0==1)
		{
			TF0=0;
			TH0=0xB8;
			TL0=0x00;
			cnt++;
			if(cnt>=7)
			{
				cnt=0;
				P0=LedChar[sec];
				sec++;
				if(sec>6)
				{
					sec=0;
				}
			}
		}
	}
}
