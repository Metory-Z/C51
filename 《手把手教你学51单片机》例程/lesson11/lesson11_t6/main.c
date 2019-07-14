/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������11�� ��ҵ��6 UART���ڿ��Ʒ������Ŀ���
* �汾�ţ�v1.0.0
* ��  ע��ֻ�����յ���д�ַ�'B'��ı�������Ŀ�/��״̬
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit BUZZ  = P1^6;

unsigned char code LedChar[] = {  //�������ʾ�ַ�ת����
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[7] = {  //�����+����LED��ʾ������
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�
unsigned char RxdByte = 0;  //���ڽ��յ����ֽ�
unsigned char flagBuzz = 0; //���������Ʊ�־

void ConfigTimer0(unsigned int ms);
void ConfigUART(unsigned int baud);

void main()
{
    EA = 1;       //ʹ�����ж�
    ENLED = 0;    //ѡ������ܺͶ���LED
    ADDR3 = 1;
    ConfigTimer0(1);   //����T0��ʱ1ms
    ConfigUART(9600);  //���ò�����Ϊ9600
    
    while (1)
    {   //�������ֽ������������ʮ��������ʽ��ʾ����
        LedBuff[0] = LedChar[RxdByte & 0x0F];
        LedBuff[1] = LedChar[RxdByte >> 4];
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
/* �������ú�����baud-ͨ�Ų����� */
void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //���ô���Ϊģʽ1
    TMOD &= 0x0F;  //����T1�Ŀ���λ
    TMOD |= 0x20;  //����T1Ϊģʽ2
    TH1 = 256 - (11059200/12/32)/baud;  //����T1����ֵ
    TL1 = TH1;     //��ֵ��������ֵ
    ET1 = 0;       //��ֹT1�ж�
    ES  = 1;       //ʹ�ܴ����ж�
    TR1 = 1;       //����T1
}
/* ����ɨ�躯�������ڶ�ʱ�ж��е��� */
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
/* T0�жϷ����������LEDɨ�� */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    LedScan();   //LEDɨ����ʾ
    if (flagBuzz == 0)
        BUZZ = 1;      //�������ر�
    else
        BUZZ = ~BUZZ;  //����������
}
/* UART�жϷ����� */
void InterruptUART() interrupt 4
{
    if (RI)  //���յ��ֽ�
    {
        RI = 0;  //�ֶ���������жϱ�־λ
        RxdByte = SBUF;  //���յ������ݱ��浽�����ֽڱ�����
        SBUF = RxdByte;  //���յ���������ֱ�ӷ��أ�����-"echo"��
                         //������ʾ�û��������Ϣ�Ƿ�����ȷ����
        if (RxdByte == 'B')  //���յ���д��ĸBʱ�ı�һ�η��������Ʊ�־
        {
            flagBuzz = !flagBuzz;
        }
    }
    if (TI)  //�ֽڷ������
    {
        TI = 0;  //�ֶ����㷢���жϱ�־λ
    }
}
