/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������10�� ����PWM��ʽ����LED������
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

unsigned char HighRH = 0;  //�ߵ�ƽ����ֵ�ĸ��ֽ�
unsigned char HighRL = 0;  //�ߵ�ƽ����ֵ�ĵ��ֽ�
unsigned char LowRH  = 0;  //�͵�ƽ����ֵ�ĸ��ֽ�
unsigned char LowRL  = 0;  //�͵�ƽ����ֵ�ĵ��ֽ�

void ConfigPWM(unsigned int fr, unsigned char dc);
void ClosePWM();

void main()
{
    unsigned int i;
    
    EA = 1;     //�����ж�
    ENLED = 0;  //ʹ�ܶ���LED
    ADDR3 = 1;
    ADDR2 = 1;
    ADDR1 = 1;
    ADDR0 = 0;
    
    while (1)
    {
        ConfigPWM(100, 10);    //Ƶ��100Hz��ռ�ձ�10%
        for (i=0; i<40000; i++);
        ClosePWM();
        ConfigPWM(100, 40);    //Ƶ��100Hz��ռ�ձ�40%
        for (i=0; i<40000; i++);
        ClosePWM();
        ConfigPWM(100, 90);    //Ƶ��100Hz��ռ�ձ�90%
        for (i=0; i<40000; i++);
        ClosePWM();            //�ر�PWM���൱��ռ�ձ�100%
        for (i=0; i<40000; i++);
    }
}
/* ���ò�����PWM��fr-Ƶ�ʣ�dc-ռ�ձ� */
void ConfigPWM(unsigned int fr, unsigned char dc)
{
    unsigned int  high, low;
    unsigned long tmp;
    
    tmp  = (11059200/12) / fr;  //����һ����������ļ���ֵ
    high = (tmp*dc) / 100;      //����ߵ�ƽ����ļ���ֵ
    low  = tmp - high;          //����͵�ƽ����ļ���ֵ
    high = 65536 - high + 12;   //����ߵ�ƽ������ֵ�������ж���ʱ
    low  = 65536 - low  + 12;   //����͵�ƽ������ֵ�������ж���ʱ
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
/* �ر�PWM */
void ClosePWM()
{
    TR0 = 0;     //ֹͣ��ʱ��
    ET0 = 0;     //��ֹ�ж�
    PWMOUT = 1;  //����ߵ�ƽ
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
