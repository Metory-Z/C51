/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������10�� ��ͨ�źŵ�ʾ��
* �汾�ţ�v1.0.0
* ��  ע���������10��10.3��
*******************************************************************************
*/

#include <reg52.h>

sbit  ADDR3 = P1^3;
sbit  ENLED = P1^4;

unsigned char code LedChar[] = {  //�������ʾ�ַ�ת����
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[7] = {  //�����+����LED��ʾ������
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
bit flag1s = 1;          //1�붨ʱ��־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

void ConfigTimer0(unsigned int ms);
void TrafficLight();

void main()
{
    EA = 1;      //�����ж�
    ENLED = 0;   //ʹ������ܺ�LED
    ADDR3 = 1;
	ConfigTimer0(1);  //����T0��ʱ1ms
    
    while (1)
    {
        if (flag1s)  //ÿ��ִ��һ�ν�ͨ��ˢ��
        {
            flag1s = 0;
            TrafficLight();
        }
    }
}
/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 13;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* ��ͨ����ʾˢ�º��� */
void TrafficLight()
{
    static unsigned char color = 2;  //��ɫ������0-��ɫ/1-��ɫ/2-��ɫ
    static unsigned char timer = 0;  //����ʱ��ʱ��
    
    if (timer == 0) //����ʱ��0ʱ���л���ͨ��
    {
        switch (color)  //LED8/9�����̵ƣ�LED5/6����Ƶƣ�LED2/3������
        {
            case 0:     //�л�����ɫ����3��
                color = 1;
                timer = 2;
                LedBuff[6] = 0xE7;
                break;
            case 1:     //�л�����ɫ����30��
                color = 2;
                timer = 29;
                LedBuff[6] = 0xFC;
                break;
            case 2:     //�л�����ɫ����40��
                color = 0;
                timer = 39;
                LedBuff[6] = 0x3F;
                break;
            default:
                break;
        }
    }
    else  //����ʱδ��0ʱ���ݼ������ֵ
    {
        timer--;
    }
    LedBuff[0] = LedChar[timer%10];  //����ʱ��ֵ��λ��ʾ
    LedBuff[1] = LedChar[timer/10];  //����ʱ��ֵʮλ��ʾ
}
/* LED��̬ɨ��ˢ�º��������ڶ�ʱ�ж��е��� */
void LedScan()
{
    static unsigned char i = 0;  //��̬ɨ������
    
    P0 = 0xFF;             //�ر����ж�ѡλ����ʾ����
    P1 = (P1 & 0xF8) | i;  //λѡ����ֵ��ֵ��P1�ڵ�3λ
    P0 = LedBuff[i];       //������������λ�õ������͵�P0��
    if (i < 6)             //��������ѭ������������������
        i++;
    else
        i = 0;
}
/* T0�жϷ����������LEDɨ����붨ʱ */
void InterruptTimer0() interrupt 1
{
    static unsigned int tmr1s = 0;  //1�붨ʱ��

    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    LedScan();   //LEDɨ����ʾ
    tmr1s++;     //1�붨ʱ�Ĵ���
    if (tmr1s >= 1000)
    {
        tmr1s = 0;
        flag1s = 1;  //�����붨ʱ��־
    }
}
