/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������11�� ��ҵ��5 UART���ڿ�����ˮ��������ֹͣ
* �汾�ţ�v1.0.0
* ��  ע��ÿ�ӵ������ֽ����ݺ󶼸ı���ˮ������/ֹͣ״̬
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

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
unsigned char flag200ms = 0;  //200ms��ʱ��־
unsigned char flagLight = 1;  //��ˮ�����б�־

void ConfigTimer0(unsigned int ms);
void ConfigUART(unsigned int baud);
void FlowingLight();

void main()
{
    EA = 1;       //ʹ�����ж�
    ENLED = 0;    //ѡ������ܺͶ���LED
    ADDR3 = 1;
    ConfigTimer0(1);   //����T0��ʱ1ms
    ConfigUART(9600);  //���ò�����Ϊ9600
    
    while (1)
    {
        if (flagLight == 0)
        {
            LedBuff[6] = 0xFF;   //Ϩ����ˮ��
        }
        else
        {
            if (flag200ms != 0)  //200msˢ��һ����ˮ��
            {
                flag200ms = 0;
                FlowingLight();
            }
        }
        //�������ֽ������������ʮ��������ʽ��ʾ����
        LedBuff[0] = LedChar[RxdByte & 0x0F];
        LedBuff[1] = LedChar[RxdByte >> 4];
    }
}
/* ��ˮ��ʵ�ֺ��� */
void FlowingLight()
{
    static unsigned char dir = 0;   //��λ�������dir�����ڿ�����λ�ķ���
    static unsigned char shift = 0x01;  //ѭ����λ����shift��������ֵ0x01

    LedBuff[6] = ~shift;      //ѭ����λ����ȡ��������8��LED
    if (dir == 0)             //��λ�������Ϊ0ʱ������
    {
        shift = shift << 1;   //ѭ����λ��������1λ
        if (shift == 0x80)    //���Ƶ�����˺󣬸ı���λ����
        {
            dir = 1;
        }
    }
    else                      //��λ���������Ϊ0ʱ������
    {
        shift = shift >> 1;   //ѭ����λ��������1λ
        if (shift == 0x01)    //���Ƶ����Ҷ˺󣬸ı���λ����
        {
            dir = 0;
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
    static unsigned char tmr200ms = 0;
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    LedScan();   //LEDɨ����ʾ
    tmr200ms++;  //200ms��ʱ
    if (tmr200ms >= 200)
    {
        tmr200ms = 0;
        flag200ms = 1;
    }
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
        flagLight = !flagLight;  //ÿ�յ�һ���ֽڸı�һ����ˮ�Ʊ�־
    }
    if (TI)  //�ֽڷ������
    {
        TI = 0;  //�ֶ����㷢���жϱ�־λ
    }
}
