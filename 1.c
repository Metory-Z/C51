#include <reg52.h>

unsigned char Send[] = "hello from 51";
unsigned char T0RH = 0;
unsigned char T0RL = 0;
bit match = 1;

void UARTSend(unsigned char Byte);
void ConfigTimer0(unsigned int ms);
void ConfigUART();
void UARTSend();

void main()
{
    unsigned int i;

    EA  = 1;
    //ConfigTimer0(1);
    ConfigUART();
    if (match)
    {
   		UARTSend();
        for (i=0; i<20000; i++);
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
        ES = 1;
    }  
}

void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;

    tmp = 11059200 / 12;
    tmp = (tmp * ms) / 1000;
    tmp = 65536 - tmp;
    tmp = tmp + 13;
    T0RH = (unsigned char)(tmp>>8);
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;
    TMOD |= 0x01;
    TH0 = T0RH;
    TL0 = T0RL;
    ET0 = 1;
    TR0 = 1;
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

void InterruptTimer0() interrupt 1
{
    TH0 = 0xFC;
    TL0 = 0x67; 
}

void InterruptUART() interrupt 4
{
	if (RI)
    {
        RI = 0;
        //if (SBUF)
            match = 1;		
    }
        //match = 0;
    if (TI)
    {
        TI = 0;		
    }
}