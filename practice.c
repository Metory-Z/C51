#include <reg52.h>

unsigned char Send[] = "hello from 51\n";
unsigned char Recieve[] = "hello from PC";
unsigned char Tmp;
unsigned char T0RH = 0;
unsigned char T0RL = 0;
bit match = 1;

void UARTSend();
void UARTSend(unsigned char Byte);
void ConfigTimer0(unsigned int ms);
void ConfigUART();

void main()
{
    unsigned int i;

    EA  = 1;
    ConfigUART();
    while (match)
    {
        if (Tmp != 0)
        {
            UARTSend();
            match = 0;
        } 
    } 
}

void UARTSend()
{
    unsigned char i;
  
    for (i=0; i<14; i++)
    {
        ES = 0;
        TI = 0;
        SBUF = Send[i];
        while(!TI);
        TI = 0;
        ES = 1;
    }  
}

void ConfigUART()
{
    SCON = 0x50;
    TMOD &= 0x0F;
    TMOD |= 0x20;
    TH1 = 0xFD;
    TL1 = 0xFD;
    ET1 = 0;
    TR1 = 1;
    ES  = 1;
}

void InterruptUART() interrupt 4
{
	if (RI)
    {
        RI = 0;
        Tmp = SBUF;
    }
}