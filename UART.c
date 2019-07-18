#include<reg52.h>

sbit PIN_RXD = P3^0;
sbit PIN_TXD = P3^1;

bit RXD_or_TXD = 0;
bit RXDEnd = 0;
bit TXDEnd = 0;
unsigned char RxdBuf = 0;
unsigned char TxdBuf = 0;

void ConfigUART(unsigned int baud);
void StartTXD(unsigned char dat);
void StartRXD();

void main()
{
    EA = 1;
    ConfigUART(9600);

    while (1)
    {
        while (PIN_RXD);
        StartRXD();
        while (!RXDEnd);
        StartTXD(RxdBuf + 1);
        while (!TXDEnd);
    }
}

void ConfigUART(unsigned int baud)
{
    TMOD &= 0xF0;
    TMOD |= 0x02;
    TH0 = 256 - (11059200/12)/baud;
}

void StartRXD()
{
    TL0 = 256 - ((256-TH0)>>1);
    ET0 = 1;
    TR0 = 1;
    RXDEnd = 0;
    RXD_or_TXD = 0;
}

void StartTXD(unsigned char dat)
{
    TxdBuf = dat;
    TL0 = TH0;
    ET0 = 1;
    TR0 = 1;
    PIN_TXD = 0;
    TXDEnd = 0;
    RXD_or_TXD = 1;
}

void InterruptTimer0() interrupt 1
{
    static unsigned char cnt = 0;

    if (RXD_or_TXD)
    {
        cnt++;
        if (cnt <= 8)
        {
            PIN_TXD = TxdBuf & 0x01;
            TxdBuf>>=1;
        }
        else if (cnt == 9)
        {
            PIN_TXD = 1;
        }
        else
        {
            cnt = 0;
            TR0 = 0;
            TXDEnd = 1;
        }
    }
    else
    {
        if (cnt == 0)
        {
            if (!PIN_RXD)
            {
                RxdBuf = 0;
                cnt++;
            }
            else
            {
                TR0 = 0;
            }
        }
        else if (cnt <= 8)
        {
            RxdBuf >>= 1;
            if (PIN_RXD)
            {
                RxdBuf |= 0x80;
            }
            cnt++;
        }
        else
        {
            cnt = 0;
            TR0 = 0;
            if (PIN_RXD)
            {
                RXDEnd = 1;
            }
        }
    } 
}