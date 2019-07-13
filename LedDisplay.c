#include<reg52.h>

sbit A0=P1^0;
sbit A1=P1^1;
sbit ADDR2=P1^2;
sbit ADDR3=P1^3;
sbit ENLED=P1^4;

void main()
{
	ENLED=0;
	ADDR3=1;
	ADDR2=0;
	A1=1;
	A0=0;
	P0=0xB9;
	while(1);
}
