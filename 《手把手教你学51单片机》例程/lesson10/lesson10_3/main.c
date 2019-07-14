/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������10�� ����PWM��ʽ����LEDʵ�ֺ�����Ч��
* �汾�ţ�v1.0.0
* ��  ע���������10��10.2��
*******************************************************************************
*/

#include <reg52.h>

sbit PWMOUT = P0^0;
sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned long PeriodCnt = 0;  //PWM���ڼ���ֵ
unsigned char HighRH = 0;  //�ߵ�ƽ����ֵ�ĸ��ֽ�
unsigned char HighRL = 0;  //�ߵ�ƽ����ֵ�ĵ��ֽ�
unsigned char LowRH  = 0;  //�͵�ƽ����ֵ�ĸ��ֽ�
unsigned char LowRL  = 0;  //�͵�ƽ����ֵ�ĵ��ֽ�
unsigned char T1RH = 0;    //T1����ֵ�ĸ��ֽ�
unsigned char T1RL = 0;    //T1����ֵ�ĵ��ֽ�

void ConfigTimer1(unsigned int ms);
void ConfigPWM(unsigned int fr, unsigned char dc);

void main()
{
    EA = 1;     //�����ж�
    ENLED = 0;  //ʹ�ܶ���LED
    ADDR3 = 1;
    ADDR2 = 1;
    ADDR1 = 1;
    ADDR0 = 0;
    
    ConfigPWM(100, 10);  //���ò�����PWM
    ConfigTimer1(50);    //��T1��ʱ����ռ�ձ�
    while (1);
}
/* ���ò�����T1��ms-��ʱʱ�� */
void ConfigTimer1(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 12;           //�����ж���Ӧ��ʱ��ɵ����
    T1RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T1RL = (unsigned char)tmp;
    TMOD &= 0x0F;   //����T1�Ŀ���λ
    TMOD |= 0x10;   //����T1Ϊģʽ1
    TH1 = T1RH;     //����T1����ֵ
    TL1 = T1RL;
    ET1 = 1;        //ʹ��T1�ж�
    TR1 = 1;        //����T1
}
/* ���ò�����PWM��fr-Ƶ�ʣ�dc-ռ�ձ� */
void ConfigPWM(unsigned int fr, unsigned char dc)
{
    unsigned int high, low;
    
    PeriodCnt = (11059200/12) / fr; //����һ����������ļ���ֵ
    high = (PeriodCnt*dc) / 100;    //����ߵ�ƽ����ļ���ֵ
    low  = PeriodCnt - high;        //����͵�ƽ����ļ���ֵ
    high = 65536 - high + 12;       //����ߵ�ƽ�Ķ�ʱ������ֵ�������ж���ʱ
    low  = 65536 - low  + 12;       //����͵�ƽ�Ķ�ʱ������ֵ�������ж���ʱ
    HighRH = (unsigned char)(high>>8); //�ߵ�ƽ����ֵ���Ϊ�ߵ��ֽ�
    HighRL = (unsigned char)high;
    LowRH  = (unsigned char)(low>>8);  //�͵�ƽ����ֵ���Ϊ�ߵ��ֽ�
    LowRL  = (unsigned char)low;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = HighRH;   //����T0����ֵ
    TL0 = HighRL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
    PWMOUT = 1;     //����ߵ�ƽ
}
/* ռ�ձȵ���������Ƶ�ʲ���ֻ����ռ�ձ� */
void AdjustDutyCycle(unsigned char dc)
{
    unsigned int  high, low;
    
    high = (PeriodCnt*dc) / 100;    //����ߵ�ƽ����ļ���ֵ
    low  = PeriodCnt - high;        //����͵�ƽ����ļ���ֵ
    high = 65536 - high + 12;       //����ߵ�ƽ�Ķ�ʱ������ֵ�������ж���ʱ
    low  = 65536 - low  + 12;       //����͵�ƽ�Ķ�ʱ������ֵ�������ж���ʱ
    HighRH = (unsigned char)(high>>8); //�ߵ�ƽ����ֵ���Ϊ�ߵ��ֽ�
    HighRL = (unsigned char)high;
    LowRH  = (unsigned char)(low>>8);  //�͵�ƽ����ֵ���Ϊ�ߵ��ֽ�
    LowRL  = (unsigned char)low;
}
/* T0�жϷ�����������PWM��� */
void InterruptTimer0() interrupt 1
{
    if (PWMOUT == 1)  //��ǰ���Ϊ�ߵ�ƽʱ��װ�ص͵�ƽֵ������͵�ƽ
    {
        TH0 = LowRH;
        TL0 = LowRL;
        PWMOUT = 0;
    }
    else              //��ǰ���Ϊ�͵�ƽʱ��װ�ظߵ�ƽֵ������ߵ�ƽ
    {
        TH0 = HighRH;
        TL0 = HighRL;
        PWMOUT = 1;
    }
}
/* T1�жϷ���������ʱ��̬����ռ�ձ� */
void InterruptTimer1() interrupt 3
{
    static bit dir = 0;
    static unsigned char index = 0;
    unsigned char code table[13] = {  //ռ�ձȵ�����
        5, 18, 30, 41, 51, 60, 68, 75, 81, 86, 90, 93, 95
    };

    TH1 = T1RH;  //���¼���T1����ֵ
    TL1 = T1RL;
    AdjustDutyCycle(table[index]); //����PWM��ռ�ձ�
    if (dir == 0)  //������ռ�ձ�
    {
        index++;
        if (index >= 12)
        {
            dir = 1;
        }
    }
    else          //�𲽼�Сռ�ձ�
    {
        index--;
        if (index == 0)
        {
            dir = 0;
        }
    }
}

