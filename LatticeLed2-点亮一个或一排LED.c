#include<reg52.h>

sbit LED=P0^0;
sbit ADDR0=P1^0;
sbit ADDR1=P1^1;
sbit ADDR2=P1^2;
sbit ADDR3=P1^3;
sbit ENLED=P1^4;

void main()
{
	ENLED=0;
	ADDR3=0;
	ADDR2=1;
	ADDR1=0;
	ADDR0=1;
	#LED=0
	P0=0x00; 
	while(1);
}
