/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������15�� DS1302���ζ�дģʽʾ��
* �汾�ţ�v1.0.0
* ��  ע���������15��15.3.4��
*         �õ��ζ�дģʽ����DS1302����������ʱ����ʾ��Һ����
*******************************************************************************
*/

#include <reg52.h>

sbit DS1302_CE = P1^7;
sbit DS1302_CK = P3^5;
sbit DS1302_IO = P3^4;

bit flag200ms = 0;       //200ms��ʱ��־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

void ConfigTimer0(unsigned int ms);
void InitDS1302();
unsigned char DS1302SingleRead(unsigned char reg);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

void main()
{
    unsigned char i;
    unsigned char psec=0xAA;  //�뱸�ݣ���ֵAAȷ���״ζ�ȡʱ����ˢ����ʾ
    unsigned char time[8];    //��ǰʱ������
    unsigned char str[12];    //�ַ���ת��������

    EA = 1;           //�����ж�
    ConfigTimer0(1);  //T0��ʱ1ms
    InitDS1302();     //��ʼ��ʵʱʱ��
    InitLcd1602();    //��ʼ��Һ��
    
    while (1)
    {
        if (flag200ms)  //ÿ200ms��ȡ����ʱ��
        {
            flag200ms = 0;
            for (i=0; i<7; i++)  //��ȡDS1302��ǰʱ��
            {
                time[i] = DS1302SingleRead(i);
            }
            if (psec != time[0]) //��⵽ʱ���б仯ʱˢ����ʾ
            {
                str[0] = '2';  //�����ݵĸ�2λ��20
                str[1] = '0';
                str[2] = (time[6] >> 4) + '0';  //���ꡱ��λ����ת��ΪASCII��
                str[3] = (time[6]&0x0F) + '0';  //���ꡱ��λ����ת��ΪASCII��
                str[4] = '-';  //������ڷָ���
                str[5] = (time[4] >> 4) + '0';  //���¡�
                str[6] = (time[4]&0x0F) + '0';
                str[7] = '-';
                str[8] = (time[3] >> 4) + '0';  //���ա�
                str[9] = (time[3]&0x0F) + '0';
                str[10] = '\0';
                LcdShowStr(0, 0, str);  //��ʾ��Һ���ĵ�һ��
                
                str[0] = (time[5]&0x0F) + '0';  //�����ڡ�
                str[1] = '\0';
                LcdShowStr(11, 0, "week");
                LcdShowStr(15, 0, str);  //��ʾ��Һ���ĵ�һ��
                
                str[0] = (time[2] >> 4) + '0';  //��ʱ��
                str[1] = (time[2]&0x0F) + '0';
                str[2] = ':';  //���ʱ��ָ���
                str[3] = (time[1] >> 4) + '0';  //���֡�
                str[4] = (time[1]&0x0F) + '0';
                str[5] = ':';
                str[6] = (time[0] >> 4) + '0';  //���롱
                str[7] = (time[0]&0x0F) + '0';
                str[8] = '\0';
                LcdShowStr(4, 1, str);  //��ʾ��Һ���ĵڶ���
                
                psec = time[0];  //�õ�ǰֵ�����ϴ�����
            }
        }
    }
}
/* ����һ���ֽڵ�DS1302ͨ�������� */
void DS1302ByteWrite(unsigned char dat)
{
    unsigned char mask;
    
    for (mask=0x01; mask!=0; mask<<=1)  //��λ��ǰ����λ�Ƴ�
    {
        if ((mask&dat) != 0) //���������λ����
            DS1302_IO = 1;
        else
            DS1302_IO = 0;
        DS1302_CK = 1;       //Ȼ������ʱ��
        DS1302_CK = 0;       //������ʱ�ӣ����һ��λ�Ĳ���
    }
    DS1302_IO = 1;           //���ȷ���ͷ�IO����
}
/* ��DS1302ͨ�������϶�ȡһ���ֽ� */
unsigned char DS1302ByteRead()
{
    unsigned char mask;
    unsigned char dat = 0;
    
    for (mask=0x01; mask!=0; mask<<=1)  //��λ��ǰ����λ��ȡ
    {
        if (DS1302_IO != 0)  //���ȶ�ȡ��ʱ��IO���ţ�������dat�еĶ�Ӧλ
        {
            dat |= mask;
        }
        DS1302_CK = 1;       //Ȼ������ʱ��
        DS1302_CK = 0;       //������ʱ�ӣ����һ��λ�Ĳ���
    }
    return dat;              //��󷵻ض������ֽ�����
}
/* �õ���д������ĳһ�Ĵ���д��һ���ֽڣ�reg-�Ĵ�����ַ��dat-��д���ֽ� */
void DS1302SingleWrite(unsigned char reg, unsigned char dat)
{
    DS1302_CE = 1;                   //ʹ��Ƭѡ�ź�
    DS1302ByteWrite((reg<<1)|0x80);  //����д�Ĵ���ָ��
    DS1302ByteWrite(dat);            //д���ֽ�����
    DS1302_CE = 0;                   //����Ƭѡ�ź�
}
/* �õ��ζ�������ĳһ�Ĵ�����ȡһ���ֽڣ�reg-�Ĵ�����ַ������ֵ-�������ֽ� */
unsigned char DS1302SingleRead(unsigned char reg)
{
    unsigned char dat;
    
    DS1302_CE = 1;                   //ʹ��Ƭѡ�ź�
    DS1302ByteWrite((reg<<1)|0x81);  //���Ͷ��Ĵ���ָ��
    dat = DS1302ByteRead();          //��ȡ�ֽ�����
    DS1302_CE = 0;                   //����Ƭѡ�ź�
    
    return dat;
}
/* DS1302��ʼ�����緢���������������ó�ʼʱ�� */
void InitDS1302()
{
    unsigned char i;
    unsigned char code InitTime[] = {  //2013��10��8�� ���ڶ� 12:30:00
        0x00,0x30,0x12, 0x08, 0x10, 0x02, 0x13
    };
    
    DS1302_CE = 0;  //��ʼ��DS1302ͨ������
    DS1302_CK = 0;
    i = DS1302SingleRead(0);  //��ȡ��Ĵ���
    if ((i & 0x80) != 0)      //����Ĵ������λCH��ֵ�ж�DS1302�Ƿ���ֹͣ
    {
        DS1302SingleWrite(7, 0x00);  //����д����������д������
        for (i=0; i<7; i++)          //����DS1302ΪĬ�ϵĳ�ʼʱ��
        {
            DS1302SingleWrite(i, InitTime[i]);
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
    tmp = tmp + 12;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* T0�жϷ�������ִ��200ms��ʱ */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr200ms = 0;
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    tmr200ms++;
    if (tmr200ms >= 200)  //��ʱ200ms
    {
        tmr200ms = 0;
        flag200ms = 1;
    }
}
