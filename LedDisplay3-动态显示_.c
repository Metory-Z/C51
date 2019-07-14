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
unsigned char LedBuff[6]={
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	};

void main()
{
	unsigned char i=0;
	unsigned int cnt=0;
	unsigned long sec=0;
	
	ENLED=0;
	ADDR3=1;
	TMOD=0x01;
	TH0=0xFC;
	TL0=0x67; // 1ms
	TR0=1;
	
	while(1)
	{
		if(TF0==1)
		{
			TF0=0;
			TH0=0xFC;
			TL0=0x67;
			cnt++;
			if(cnt>=1000)
			{
				cnt=0;
				sec++;
				LedBuff[0]=LedChar[sec%10];
				LedBuff[1]=LedChar[sec/10%10];
				LedBuff[2]=LedChar[sec/100%10];
				LedBuff[3]=LedChar[sec/1000%10];
				LedBuff[4]=LedChar[sec/10000%10];
				LedBuff[5]=LedChar[sec/100000%10];
				// 1s 计算一次
			}
			if(i==0)
			{ADDR2=0;ADDR1=0;ADDR0=0;i++;P0=LedBuff[0];}
			else if(i==1)
			{ADDR2=0;ADDR1=0;ADDR0=1;i++;P0=LedBuff[1];}
			else if(i==2)
			{ADDR2=0;ADDR1=1;ADDR0=0;i++;P0=LedBuff[2];}
			else if(i==3)
			{ADDR2=0;ADDR1=1;ADDR0=1;i++;P0=LedBuff[3];}
			else if(i==4)
			{ADDR2=1;ADDR1=0;ADDR0=0;i++;P0=LedBuff[4];}
			else if(i==5)
			{ADDR2=1;ADDR1=0;ADDR0=1;i=0;P0=LedBuff[5];}
		}
	}
}
