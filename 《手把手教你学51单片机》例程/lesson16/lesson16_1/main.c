/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������16�� ����ң��������ʾ��
* �汾�ţ�v1.0.0
* ��  ע���������16��16.3��
*         ���ղ�����NECЭ��ĺ�����룬�����û���ͼ�����ʾ���������
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code LedChar[] = {  //�������ʾ�ַ�ת����
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = {  //�������ʾ������
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

extern bit irflag;
extern unsigned char ircode[4];
extern void InitInfrared();
void ConfigTimer0(unsigned int ms);

void main()
{
    EA = 1;      //�����ж�
    ENLED = 0;   //ʹ��ѡ�������
    ADDR3 = 1;
    InitInfrared();   //��ʼ�����⹦��
    ConfigTimer0(1);  //����T0��ʱ1ms
    //PT0 = 1;        //����T0�ж�Ϊ�����ȼ������ñ��п���������ʱ����˸
    
    while (1)
    {
        if (irflag)  //���յ���������ʱˢ����ʾ
        {
            irflag = 0;
            LedBuff[5] = LedChar[ircode[0] >> 4];  //�û�����ʾ
            LedBuff[4] = LedChar[ircode[0]&0x0F];
            LedBuff[1] = LedChar[ircode[2] >> 4];  //������ʾ
            LedBuff[0] = LedChar[ircode[2]&0x0F];
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
/* LED��̬ɨ��ˢ�º��������ڶ�ʱ�ж��е��� */
void LedScan()
{
    static unsigned char i = 0;  //��̬ɨ������
    
    P0 = 0xFF;                  //�ر����ж�ѡλ����ʾ����
    P1 = (P1 & 0xF8) | i;       //λѡ����ֵ��ֵ��P1�ڵ�3λ
    P0 = LedBuff[i];            //������������λ�õ������͵�P0��
    if (i < sizeof(LedBuff)-1)  //��������ѭ������������������
        i++;
    else
        i = 0;
}
/* T0�жϷ�������ִ�������ɨ����ʾ */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    LedScan();   //�����ɨ����ʾ
}
