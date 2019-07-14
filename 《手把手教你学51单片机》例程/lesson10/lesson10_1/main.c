/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������10�� �����������ʾ���������
* �汾�ţ�v1.0.0
* ��  ע���������10��10.1��
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit KEY1 = P2^4;
sbit KEY2 = P2^5;
sbit KEY3 = P2^6;
sbit KEY4 = P2^7;

unsigned char code LedChar[] = {  //�������ʾ�ַ�ת����
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = {  //�������ʾ������
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char KeySta[4] = {  //������ǰ״̬
    1, 1, 1, 1
};

bit StopwatchRunning = 0;  //������б�־
bit StopwatchRefresh = 1;  //������ˢ�±�־
unsigned char DecimalPart = 0;  //����С������
unsigned int  IntegerPart = 0;  //������������
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

void ConfigTimer0(unsigned int ms);
void StopwatchDisplay();
void KeyDriver();

void main()
{
    EA = 1;      //�����ж�
    ENLED = 0;   //ʹ��ѡ�������
    ADDR3 = 1;
    P2 = 0xFE;   //P2.0��0��ѡ���4�а�����Ϊ��������
	ConfigTimer0(2);  //����T0��ʱ2ms
    
    while (1)
    {
        if (StopwatchRefresh)  //��Ҫˢ�����ʾ��ʱ������ʾ����
        {
            StopwatchRefresh = 0;
            StopwatchDisplay();
        }
        KeyDriver();  //���ð�����������
    }
}
/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 18;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* ��������ʾ���� */
void StopwatchDisplay()
{
	signed char i;
	unsigned char buf[4];  //����ת���Ļ�����
    
    //С������ת������2λ
    LedBuff[0] = LedChar[DecimalPart%10];
    LedBuff[1] = LedChar[DecimalPart/10];
    //��������ת������4λ
    buf[0] = IntegerPart%10;
    buf[1] = (IntegerPart/10)%10;
    buf[2] = (IntegerPart/100)%10;
    buf[3] = (IntegerPart/1000)%10;
    for (i=3; i>=1; i--)  //�������ָ�λ��0ת��Ϊ���ַ�
    {
        if (buf[i] == 0)
            LedBuff[i+2] = 0xFF;
        else
            break;
    }
    for ( ; i>=0; i--)  //��Ч����λת��Ϊ��ʾ�ַ�
    {
        LedBuff[i+2] = LedChar[buf[i]];
    }
    LedBuff[2] &= 0x7F;  //����С����
}
/* �����ͣ���� */
void StopwatchAction()
{
    if (StopwatchRunning)    //��������ֹͣ
        StopwatchRunning = 0;
    else                     //δ����������
        StopwatchRunning = 1;
}
/* ���λ���� */
void StopwatchReset()
{
    StopwatchRunning = 0;  //ֹͣ���
    DecimalPart = 0;       //�������ֵ
    IntegerPart = 0;
    StopwatchRefresh = 1;  //��ˢ�±�־
}
/* ����������������ⰴ��������������Ӧ����������������ѭ���е��� */
void KeyDriver()
{
    unsigned char i;
    static unsigned char backup[4] = {1,1,1,1};

    for (i=0; i<4; i++)  //ѭ�����4������
    {
        if (backup[i] != KeySta[i])  //��ⰴ������
        {
            if (backup[i] != 0)      //��������ʱִ�ж���
            {
                if (i == 1)          //Esc����λ���
                    StopwatchReset();
                else if (i == 2)     //�س�����ͣ���
                    StopwatchAction();
            }
            backup[i] = KeySta[i];   //ˢ��ǰһ�εı���ֵ
        }
    }
}
/* ����ɨ�躯�������ڶ�ʱ�ж��е��� */
void KeyScan()
{
    unsigned char i;
    static unsigned char keybuf[4] = {  //����ɨ�軺����
        0xFF, 0xFF, 0xFF, 0xFF
    };
    
    //����ֵ���뻺����
    keybuf[0] = (keybuf[0] << 1) | KEY1;
    keybuf[1] = (keybuf[1] << 1) | KEY2;
    keybuf[2] = (keybuf[2] << 1) | KEY3;
    keybuf[3] = (keybuf[3] << 1) | KEY4;
    //��������°���״̬
    for (i=0; i<4; i++)
    {
        if (keybuf[i] == 0x00)
        {   //����8��ɨ��ֵΪ0����16ms�ڶ��ǰ���״̬ʱ������Ϊ�������ȶ��İ���
            KeySta[i] = 0;
        }
        else if (keybuf[i] == 0xFF)
        {   //����8��ɨ��ֵΪ1����16ms�ڶ��ǵ���״̬ʱ������Ϊ�������ȶ��ĵ���
            KeySta[i] = 1;
        }
    }
}
/* ����ܶ�̬ɨ��ˢ�º��������ڶ�ʱ�ж��е��� */
void LedScan()
{
    static unsigned char i = 0;  //��̬ɨ������
    
    P0 = 0xFF;             //�ر����ж�ѡλ����ʾ����
    P1 = (P1 & 0xF8) | i;  //λѡ����ֵ��ֵ��P1�ڵ�3λ
    P0 = LedBuff[i];       //������������λ�õ������͵�P0��
    if (i < 5)             //��������ѭ������������������
        i++;
    else
        i = 0;
}
/* ������������ÿ��10ms����һ�ν����������ۼ� */
void StopwatchCount()
{
    if (StopwatchRunning)  //����������״̬ʱ��������ֵ
    {
        DecimalPart++;           //С������+1
        if (DecimalPart >= 100)  //С�����ּƵ�100ʱ��λ����������
        {
            DecimalPart = 0;
            IntegerPart++;       //��������+1
            if (IntegerPart >= 10000)  //�������ּƵ�10000ʱ����
            {
                IntegerPart = 0;
            }
        }
        StopwatchRefresh = 1;    //����������ˢ�±�־
    }
}
/* T0�жϷ��������������ܡ�����ɨ���������� */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr10ms = 0;

    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    LedScan();   //�����ɨ����ʾ
    KeyScan();   //����ɨ��
    //��ʱ10ms����һ��������
    tmr10ms++;
    if (tmr10ms >= 5)
    {
        tmr10ms = 0;
        StopwatchCount();  //��������������
    }
}
