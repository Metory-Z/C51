/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������10�� ��ҵ��3 ����ܼ�ʱ����ˮ��ͬʱ���е�ʾ��
* �汾�ţ�v1.0.0
* ��  ע������������ʱ��������������ˮ�ƹ��ܣ���ʾ�������ͬʱ����
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code LedChar[] = {  //�������ʾ�ַ�ת����
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[7] = {  //�����+����LED��ʾ����������ֵ0xFFȷ������ʱ������
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char cnt200 = 0;  //200ms����
unsigned int cnt1000 = 0;  //1000ms����

void ShowCount();
void FlowingLight();

void main()
{
    EA = 1;       //ʹ�����ж�
    ENLED = 0;    //ʹ��U3
    ADDR3 = 1;    //��Ϊ��Ҫ��̬�ı�ADDR0-2��ֵ�����Բ���Ҫ�ٳ�ʼ����
    TMOD = 0x01;  //����T0Ϊģʽ1
    TH0  = 0xFC;  //ΪT0����ֵ0xFC67����ʱ1ms
    TL0  = 0x67;
    ET0  = 1;     //ʹ��T0�ж�
    TR0  = 1;     //����T0
    
    while (1)
    {
        if (cnt200 >= 200)  //200msˢ��һ����ˮ��
        {
            cnt200 = 0;
            FlowingLight();
        }
        if (cnt1000 >= 1000)  //1sˢ��һ�μ���ֵ
        {
            cnt1000 = 0;
            ShowCount();
        }
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
/* �������ʾ���� */
void ShowCount()
{
    char i;  //ѭ������
    unsigned char buf[6];   //�м�ת��������
    static unsigned long sec = 0;  //��¼����������
    
    sec++;  //������Լ�1
    buf[0] = sec%10;  //��sec��ʮ����λ�ӵ͵���������ȡ��buf������
    buf[1] = sec/10%10;
    buf[2] = sec/100%10;
    buf[3] = sec/1000%10;
    buf[4] = sec/10000%10;
    buf[5] = sec/100000%10;
    for (i=5; i>=1; i--)  //�����Ϊ��ʼ������0����ʾ��������0�˳�ѭ��
    {
        if (buf[i] == 0)
            LedBuff[i] = 0xFF;
        else
            break;
    }
    for ( ; i>=0; i--)  //��ʣ�����Ч����λ��ʵת��
    {
        LedBuff[i] = LedChar[buf[i]];
    }
}
/* ��ʱ��0�жϷ����� */
void InterruptTimer0() interrupt 1
{
    static unsigned char i = 0;   //��̬ɨ�������

    TH0 = 0xFC;  //���¼��س�ֵ
    TL0 = 0x67;
    cnt200++;
    cnt1000++;
    //���´����������ܶ�̬ɨ��ˢ��
    P0 = 0xFF;   //��ʾ����
    switch (i)
    {
        case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=LedBuff[0]; break;
        case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=LedBuff[1]; break;
        case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=LedBuff[2]; break;
        case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=LedBuff[3]; break;
        case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=LedBuff[4]; break;
        case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=LedBuff[5]; break;
        case 6: ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=LedBuff[6]; break;
        default: break;
    }
}
